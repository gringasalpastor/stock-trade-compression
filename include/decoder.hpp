#pragma once

#include "trade.hpp"
#include "huffman.hpp"
#include "stats_collector.hpp"
#include <bitset>
#include <boost/noncopyable.hpp>
#include <boost/utility/value_init.hpp>
#include <boost/optional.hpp>
#include <type_traits>
#include <memory>
#include <set>

namespace exercise
{   
    /*
     * This class keeps track of everything needed to decode the input stream, and send it to
     * the output stream.
     * 
     * Tables that is decodes are generic, and it is easy to support for new types. Just create a
     * new 'read_symbol' overload. You also, have to make a prefix map 
     * (std::map<std::vector<bool>, Type>) for the given type. 
     */
    struct decoder : boost::noncopyable
    {
        /*
         * Creates a new decoder objects from a pair istream and ostream's
         */
        decoder(std::shared_ptr<std::istream> in_stream_, std::shared_ptr<std::ostream> out_stream_): 
        in_stream(in_stream_),
        out_stream(out_stream_),
        number_of_trades(0),
        buffer_bit_index(0)
        {}
    
        /*
         * Decodes all of the headers and data. Original file will be reconstructed after with is called
         */
        void decode()
        {
            decode_header();
            
            u_int64_t trade_index = 0;
            stats_collector trade_stats;
            while(trade_index < number_of_trades)
            {
                const auto trade_ = decode_trade(trade_stats);
                (*out_stream) << trade_.to_csv();
                trade_index++;
                trade_stats.add_trade(trade_);
            }
        }
        
    private:
  
        /*
         * Creates all of the Huffman prefix mappings from the header information
         */
        void decode_header()
        {            
            read(number_of_trades);
            
            //Make a maps that go from prefix to symbol.
            auto symbol_map = decode_table<uint32_t, std::string>();
            for(const auto& p : *symbol_map)
                symbol_table[p.second] = p.first;
            
            auto exchange_newline_map = decode_table<uint32_t, std::pair<char, char>>();
            for(const auto& p : *exchange_newline_map)
                exchange_table[p.second] = p.first;
            
            auto side_map = decode_table<uint32_t, char>();
            for(const auto& p : *side_map)
                side_table[p.second] = p.first;
            
            auto condition_map = decode_table<uint32_t, char>();
            for(const auto& p : *condition_map)
                condition_table[p.second] = p.first;
            
            auto send_map = decode_table<uint32_t, int64_t>();
            for(const auto& p : *send_map)
                send_table[p.second] = p.first;
            
            auto receive_map = decode_table<uint32_t, int64_t>();
            for(const auto& p : *receive_map)
                receive_table[p.second] = p.first;
            
            auto price_map = decode_table<uint32_t, int32_t>();
            for(const auto& p : *price_map)
                price_table[p.second] = p.first;
            
            auto price_overflow_map = decode_table<uint32_t, std::string>();
            for(const auto& p : *price_overflow_map)
                price_overflow_table[p.second] = p.first;
            
            auto quantity_map = decode_table<uint32_t, int32_t>();
            for(const auto& p : *quantity_map)
                quantity_table[p.second] = p.first;
        }
        
        /*
         * Takes a symbol of type Symbol and decodes it from the stream, using the 'symbol_map'
         * as it's prefix mappings
         */
        template<class Symbol>
        void decode_symbol(const std::map<std::vector<bool>, Symbol>& symbol_map, Symbol& value)
        {
            std::vector<bool> prefix;
            while(symbol_map.find(prefix) == boost::end(symbol_map))
                prefix.push_back(next_bit());
            value = symbol_map.find(prefix)->second;
        }
        
        /*
        * Reads and decodes a single trade from the input stream. return the decoded trade.
        */
        trade decode_trade(const stats_collector& trade_stats)
        {
            trade trade_ = boost::initialized_value;
            decode_symbol(symbol_table, trade_.symbol);
            decode_symbol(exchange_table, trade_.exchange_new_line);
            decode_symbol(side_table, trade_.side);
            decode_symbol(condition_table, trade_.condition);
            
            decode_symbol(send_table, trade_.send_time);            
            trade_.send_time += trade_stats.predict_send_time();
                
            decode_symbol(receive_table, trade_.receive_time);
            trade_.receive_time += trade_.send_time + trade_stats.predict_received_time_diff();
            
            decode_symbol(price_table, trade_.price);
            trade_.price += trade_stats.predict_price(trade_.symbol);
            
            decode_symbol(price_overflow_table, trade_.overflow_price_digits);
            
            decode_symbol(quantity_table, trade_.quantity);
            trade_.quantity += trade_stats.predict_quantity(trade_.symbol);
            
            return trade_;
        }
        
        /*
         * Decoding must happen 1 bit at a time. This function takes the next bit from the input stream
         */
        bool next_bit()
        {
            uint64_t byte_index = buffer_bit_index / 8;   // Byte we are currently looking at
            int local_bit_index = (buffer_bit_index % 8); // Index within the byte
            
            if(byte_index >= read_buffer.size())
            {
                static const auto buffer_size = 1048576; //1 Megabyte
                read_buffer.resize(buffer_size);
                buffer_bit_index = 0;
                byte_index = 0;
                local_bit_index = 0;
                
                //NOTE: This should be safe since they are the same size
                in_stream->read(reinterpret_cast<char*>(read_buffer.data()), read_buffer.size());
                read_buffer.resize(in_stream->gcount());
            }
            std::bitset<8> b_set(read_buffer[byte_index]);
            
            buffer_bit_index++;
            return b_set[7 - local_bit_index];
        }
        
        std::shared_ptr<std::istream> in_stream;   // Input stream
        std::shared_ptr<std::ostream> out_stream;  // Output stream
        uint64_t number_of_trades;                 // Gets set after reading the headers
        
        /* 
         * Prefix encoding for a given symbol. Maps a sequence of bits back to symbols.
         */
        std::map<std::vector<bool>, std::string> symbol_table;
        std::map<std::vector<bool>, std::pair<char, char>> exchange_table;
        std::map<std::vector<bool>, char> side_table;
        std::map<std::vector<bool>, char> condition_table;
        std::map<std::vector<bool>, int64_t> send_table;
        std::map<std::vector<bool>, int64_t> receive_table;
        std::map<std::vector<bool>, int32_t> price_table;
        std::map<std::vector<bool>, std::string> price_overflow_table;
        std::map<std::vector<bool>, int32_t> quantity_table;
        std::vector<uint8_t> read_buffer;
        /* 
         * Index of the current bit we are decoding. 
         * NOTE: Tables with 1 element use an empty sequence, as to not use up space
         */
        uint64_t buffer_bit_index;
        
        /*
        * Read in sizeof(Type) bytes from `in_stream` and return the value of the data read.
        * Type must be a pod type.
        */
        template<class Type> typename std::enable_if<std::is_pod<Type>::value, void>::type
        /*void*/ read(Type& value)
        {
            //NOTE: This should be safe since we have a pod type.
            in_stream->read(reinterpret_cast<char*>(&value), sizeof(Type));
        }
        
        /*
         * Decodes a single table. The way each table is stored depends on the Symbol. For example,
         * 'std::string' uses null terminators, while 'char' does not.
         */
        template<class SizeType, class Symbol>
        std::shared_ptr<std::map<Symbol, std::vector<bool>>> decode_table()
        {
            SizeType table_size = {};
            read(table_size);
            
            std::set<huffman::symbol_data<Symbol>, huffman::detail::symbol_data_less_f<Symbol>> symbol_data_;
            for(SizeType i = 0; i< table_size; i++)
            {
                Symbol s = {};
                read_symbol(s);
                SizeType prefix_size = {};
                read(prefix_size);
                symbol_data_.insert(huffman::make_symbol_data(s, prefix_size));
            }
            
            return huffman::create_canonical_prefix_map(symbol_data_);
        }
            
        /*
         * Overload for reading a string from the header
         */
        void read_symbol(std::string& s)
        {
            s = {};
            char c = {};
            while(true)
            {
                read(c);
                if(c == '\0')
                    break;
                s.push_back(c);
            }
        }
        
        /*
         * Overload for reading any pod type from the table
         */
        template<class Type>
        void read_symbol(Type& s)
        {
            read(s);
        }
        
        /*
         * Overload for reading an std::pair from (std::pair is not pod type)
         */
        template<class TypeA, class TypeB>
        void read_symbol(std::pair<TypeA, TypeB>& s)
        {
            read(s.first);
            read(s.second);
        }
    };
    
    /*
     * Factory to create a decoder object
     */
    std::shared_ptr<decoder> make_decoder(std::shared_ptr<std::istream> in_stream, std::shared_ptr<std::ostream> out_stream)
    {
        return std::make_shared<decoder>(in_stream, out_stream);
    }
}