#ifndef COMPRESS_ENCODER_H
#define COMPRESS_ENCODER_H

#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>
#include <fstream>
#include <map>
#include <memory>

#include "bits.hpp"
#include "csv_trade_source.hpp"
#include "huffman.hpp"
#include "stats_collector.hpp"
#include "trade.hpp"

namespace compress {

namespace detail {
/*
 * Convert variable of type `Type` to a vector of bytes.
 * Note: `Type` must be a POD type.
 */
template <class Type>
typename std::enable_if<std::is_pod<Type>::value, std::vector<uint8_t>>::type
/*std::vector<uint8_t>*/
to_bytes(const Type& value) {
  std::vector<uint8_t> bytes(reinterpret_cast<const uint8_t*>(&value),
                             reinterpret_cast<const uint8_t*>(&value) + static_cast<uint8_t>(sizeof(Type)));
  return bytes;
}

/*
 * Overload for pair of POD types
 * Note: `TypeA` and `TypeB` must be POD types
 */
template <class TypeA, class TypeB>
std::vector<uint8_t> to_bytes(const std::pair<TypeA, TypeB>& pair) {
  std::vector<uint8_t> data;
  auto                 bytes = to_bytes(pair.first);
  data.insert(boost::end(data), boost::begin(bytes), boost::end(bytes));
  bytes = to_bytes(pair.second);
  data.insert(boost::end(data), boost::begin(bytes), boost::end(bytes));
  return data;
}

/*
 * Overload for converting a string to a vector of bytes
 */
std::vector<uint8_t> to_bytes(const std::string& value) {
  std::vector<uint8_t> data;
  for (const auto char_ : value) data.push_back(to_bytes(char_).front());
  data.push_back('\0');  // End of string
  return data;
}

/*
 * Converts the prefix map to bytes that can be written to the file
 */
template <class SizeType, class Symbol>
std::vector<uint8_t> encode_table(const std::map<Symbol, long>& symbol_count, const uint64_t num_trades) {
  std::vector<uint8_t> data;
  SizeType             table_size = symbol_count.size();
  auto                 bytes      = to_bytes(table_size);
  data.insert(boost::end(data), boost::begin(bytes), boost::end(bytes));
  auto prefix_map = compress::huffman::create_prefix_map<Symbol>(
      compress::huffman::make_huffman_tree<Symbol>(symbol_count, num_trades));

  // Store the symbol table
  for (const auto& p : prefix_map) {
    // Save the symbol
    bytes = to_bytes(p.first);
    data.insert(boost::end(data), boost::begin(bytes), boost::end(bytes));

    // Save off the prefix size
    SizeType prefix_size = p.second.size();
    bytes                = to_bytes(prefix_size);
    data.insert(boost::end(data), boost::begin(bytes), boost::end(bytes));
  }
  return data;
}
}  // namespace detail

/*
 * The encoder make 2 passes over the data stream - `encode_header` and `encode_trades`. We can't encode a value until
 * we have constructed the Huffman coding (headers), so it requires 2 passes. Data is written out to the stream in
 * chunks of 1 MB.
 *
 *  - `encode_header` - first pass
 * This creates the headers using a Huffman coding. It collects frequency information and all values are pre-encoded
 * using a prediction value and difference between actual and expected.
 *
 *   - `encode_trades` - second pass
 * This uses the pre-encoded values mentioned in the first pass and applies the Huffman coding.
 */
struct encoder : boost::noncopyable {
  encoder(const boost::filesystem::path& input_path_, const boost::filesystem::path& output_path_)
      : input_path(input_path_),
        out_stream(output_path_.string(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc),
        symbol_header_size(0),
        exchange_header_size(0),
        side_header_size(0),
        condition_header_size(0),
        send_time_header_size(0),
        receive_time_diff_header_size(0),
        price_header_size(0),
        price_overflow_header_size(0),
        quantity_header_size(0) {}

  /*
   * Write all of the prefix headers
   */
  std::size_t encode_header() {
    // Mapping from symbol to how often it is seen in the file
    std::map<std::string, long>           symbol_count;
    std::map<std::pair<char, char>, long> exchange_newline_count;
    std::map<char, long>                  side_count;
    std::map<char, long>                  condition_count;
    std::map<int64_t, long>               send_time_count;
    std::map<int64_t, long>               receive_time_diff_count;
    std::map<int32_t, long>               price_count;
    std::map<std::string, long>           price_overflow_count;
    std::map<int32_t, long>               quantity_count;

    // Determine how often symbols are seen in the file
    uint64_t        num_trades = {};
    stats_collector trade_stats;
    for (const auto& trade_ : compress::make_csv_source_range(input_path)) {
      symbol_count[trade_.symbol]++;
      exchange_newline_count[trade_.exchange_new_line]++;
      side_count[trade_.side]++;
      condition_count[trade_.condition]++;
      send_time_count[trade_.send_time - trade_stats.predict_send_time()]++;
      receive_time_diff_count[(trade_.receive_time - trade_.send_time) - trade_stats.predict_received_time_diff()]++;
      price_count[trade_.price - trade_stats.predict_price(trade_.symbol)]++;
      price_overflow_count[trade_.overflow_price_digits]++;
      quantity_count[trade_.quantity - trade_stats.predict_quantity(trade_.symbol)]++;

      num_trades++;
      trade_stats.add_trade(trade_);
    }

    // Make a Canonical prefix encoding so we know how to reconstruct it later
    symbol_prefix_map = huffman::create_canonical_prefix_map(
        huffman::create_prefix_map(huffman::make_huffman_tree(symbol_count, num_trades)));
    exchange_prefix_map = huffman::create_canonical_prefix_map(
        huffman::create_prefix_map(huffman::make_huffman_tree(exchange_newline_count, num_trades)));
    side_prefix_map = huffman::create_canonical_prefix_map(
        huffman::create_prefix_map(huffman::make_huffman_tree(side_count, num_trades)));
    condition_prefix_map = huffman::create_canonical_prefix_map(
        huffman::create_prefix_map(huffman::make_huffman_tree(condition_count, num_trades)));
    send_time_prefix_map = huffman::create_canonical_prefix_map(
        huffman::create_prefix_map(huffman::make_huffman_tree(send_time_count, num_trades)));
    received_time_diff_prefix_map = huffman::create_canonical_prefix_map(
        huffman::create_prefix_map(huffman::make_huffman_tree(receive_time_diff_count, num_trades)));
    price_prefix_map = huffman::create_canonical_prefix_map(
        huffman::create_prefix_map(huffman::make_huffman_tree(price_count, num_trades)));
    price_overflow_prefix_map = huffman::create_canonical_prefix_map(
        huffman::create_prefix_map(huffman::make_huffman_tree(price_overflow_count, num_trades)));
    quantity_prefix_map = huffman::create_canonical_prefix_map(
        huffman::create_prefix_map(huffman::make_huffman_tree(quantity_count, num_trades)));

    std::vector<uint8_t> header_data;

    // Store the number of trades
    auto bytes = detail::to_bytes(num_trades);
    header_data.insert(boost::end(header_data), boost::begin(bytes), boost::end(bytes));

    // Store the symbol table
    bytes              = detail::encode_table<uint32_t>(symbol_count, num_trades);
    symbol_header_size = bytes.size();
    header_data.insert(boost::end(header_data), boost::begin(bytes), boost::end(bytes));

    // Store the exchange table
    bytes                = detail::encode_table<uint32_t>(exchange_newline_count, num_trades);
    exchange_header_size = bytes.size();
    header_data.insert(boost::end(header_data), boost::begin(bytes), boost::end(bytes));

    // Store the side table
    bytes            = detail::encode_table<uint32_t>(side_count, num_trades);
    side_header_size = bytes.size();
    header_data.insert(boost::end(header_data), boost::begin(bytes), boost::end(bytes));

    // Store the condition table
    bytes                 = detail::encode_table<uint32_t>(condition_count, num_trades);
    condition_header_size = bytes.size();
    header_data.insert(boost::end(header_data), boost::begin(bytes), boost::end(bytes));

    // Store the send_time table
    bytes                 = detail::encode_table<uint32_t>(send_time_count, num_trades);
    send_time_header_size = bytes.size();
    header_data.insert(boost::end(header_data), boost::begin(bytes), boost::end(bytes));

    // Store the receive time diff table
    bytes                         = detail::encode_table<uint32_t>(receive_time_diff_count, num_trades);
    receive_time_diff_header_size = bytes.size();
    header_data.insert(boost::end(header_data), boost::begin(bytes), boost::end(bytes));

    // Store the price table
    bytes             = detail::encode_table<uint32_t>(price_count, num_trades);
    price_header_size = bytes.size();
    header_data.insert(boost::end(header_data), boost::begin(bytes), boost::end(bytes));

    // Store the price overflow table
    bytes                      = detail::encode_table<uint32_t>(price_overflow_count, num_trades);
    price_overflow_header_size = bytes.size();
    header_data.insert(boost::end(header_data), boost::begin(bytes), boost::end(bytes));

    // Store the quantity table
    bytes                = detail::encode_table<uint32_t>(quantity_count, num_trades);
    quantity_header_size = bytes.size();
    header_data.insert(boost::end(header_data), boost::begin(bytes), boost::end(bytes));

    // Write the header
    write_bytes(header_data.data(), header_data.size());
    return header_data.size();
  }

  /*
   * Write all of the encoded trades
   */
  void encode_trades() {
    static const auto flush_size = 1048576;  // 1 MB

    // Allows for conversion from bits to bytes
    auto converter = make_bits_to_bytes_converter();

    stats_collector trade_stats;
    for (const auto& trade_ : compress::make_csv_source_range(input_path)) {
      converter->append_bits(symbol_prefix_map[trade_.symbol]);
      converter->append_bits(exchange_prefix_map[trade_.exchange_new_line]);
      converter->append_bits(side_prefix_map[trade_.side]);
      converter->append_bits(condition_prefix_map[trade_.condition]);
      converter->append_bits(send_time_prefix_map[trade_.send_time - trade_stats.predict_send_time()]);
      converter->append_bits(received_time_diff_prefix_map[(trade_.receive_time - trade_.send_time) -
                                                           trade_stats.predict_received_time_diff()]);
      converter->append_bits(price_prefix_map[trade_.price - trade_stats.predict_price(trade_.symbol)]);
      converter->append_bits(price_overflow_prefix_map[trade_.overflow_price_digits]);
      converter->append_bits(quantity_prefix_map[trade_.quantity - trade_stats.predict_quantity(trade_.symbol)]);

      if (converter->number_of_fully_packed_bytes() >= flush_size) {
        write_bytes(converter->data().data(), converter->number_of_fully_packed_bytes());
        converter->clear_fully_packed_bytes();
      }

      trade_stats.add_trade(trade_);
    }

    // Write out all of the remaining bytes. Note, the last byte might be
    // partially filled.
    write_bytes(converter->data().data(), converter->data().size());
    out_stream.close();
  }

  std::size_t get_symbol_header_size() const { return symbol_header_size; }
  std::size_t get_exchange_newline_header_size() const { return exchange_header_size; }
  std::size_t get_side_header_size() const { return side_header_size; }
  std::size_t get_condition_header_size() const { return condition_header_size; }
  std::size_t get_send_time_header_size() const { return send_time_header_size; }
  std::size_t get_receive_time_diff_header_size() const { return receive_time_diff_header_size; }
  std::size_t get_price_header_size() const { return price_header_size; }
  std::size_t get_price_overflow_header_size() const { return price_overflow_header_size; }
  std::size_t get_quantity_header_size() const { return quantity_header_size; }

 private:
  /*
   * Wraps the ostream write function as to only have one place to do `reinterpret_cast`
   */
  void write_bytes(const uint8_t* data, const std::size_t size) {
    // NOTE: conversion from uint8_t* to char* is safe(same size)
    static_assert(sizeof(uint8_t) == sizeof(char), "sizeof(uint8_t) != sizeof(char)");
    out_stream.write(reinterpret_cast<const char*>(data), size);
  }

  boost::filesystem::path input_path;
  std::ofstream           out_stream;

  // Mapping from symbol to its Huffman prefix
  std::map<std::string, std::vector<bool>>           symbol_prefix_map;
  std::map<std::pair<char, char>, std::vector<bool>> exchange_prefix_map;
  std::map<char, std::vector<bool>>                  side_prefix_map;
  std::map<char, std::vector<bool>>                  condition_prefix_map;
  std::map<int64_t, std::vector<bool>>               send_time_prefix_map;
  std::map<int64_t, std::vector<bool>>               received_time_diff_prefix_map;
  std::map<int32_t, std::vector<bool>>               price_prefix_map;
  std::map<std::string, std::vector<bool>>           price_overflow_prefix_map;
  std::map<int32_t, std::vector<bool>>               quantity_prefix_map;

  // Size of each header
  std::size_t symbol_header_size;
  std::size_t exchange_header_size;
  std::size_t side_header_size;
  std::size_t condition_header_size;
  std::size_t send_time_header_size;
  std::size_t receive_time_diff_header_size;
  std::size_t price_header_size;
  std::size_t price_overflow_header_size;
  std::size_t quantity_header_size;
};

}  // namespace compress

#endif
