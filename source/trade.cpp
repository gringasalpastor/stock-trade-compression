#include "trade.hpp"
namespace compress {

trade make_trade(const std::string& symbol_, const std::pair<char, char>& exchange_new_line_, const char side_,
                 const char condition_, const int64_t send_time_, const int64_t receive_time_, const int32_t price_,
                 const std::string& overflow_price_digits_, const int32_t quantity_) {
  trade t;
  t.symbol                = symbol_;
  t.exchange_new_line     = exchange_new_line_;
  t.side                  = side_;
  t.condition             = condition_;
  t.send_time             = send_time_;
  t.receive_time          = receive_time_;
  t.price                 = price_;
  t.overflow_price_digits = overflow_price_digits_;
  t.quantity              = quantity_;

  return t;
}

std::string trade::to_csv() const {
  int32_t integer_part = std::abs(price) / 100;

  auto        local_overflow_price_digits = overflow_price_digits;
  std::string minus_str                   = "";
  if (local_overflow_price_digits.size() > 0 && local_overflow_price_digits[0] == '-') {
    minus_str                   = "-";
    local_overflow_price_digits = overflow_price_digits.substr(1);  // Remove the leading '-'
  }

  std::string decimal_part = std::to_string(std::abs(price) - integer_part * 100);

  // Add 0's till we have 2 digits
  while (decimal_part.size() < 2) decimal_part.insert(boost::begin(decimal_part), '0');

  decimal_part = "." + decimal_part + local_overflow_price_digits;

  // Remove trailing 0's
  while (decimal_part[decimal_part.size() - 1] == '0') decimal_part.pop_back();

  if (decimal_part == ".") decimal_part = "";

  std::string integer_part_str = std::to_string(integer_part);

  static const boost::format format("%s,%c,%c,%c,%d,%d,%s,%d");
  std::stringstream          ss;
  ss << boost::format(format) % symbol % exchange_new_line.first % side % condition % send_time % receive_time %
            (minus_str + integer_part_str + decimal_part) % quantity;

  /*
   * Don't use the system new lines, we could have a mix of new line types in
   * the file NOTE: '\r' really means "\r\n"
   */
  if (exchange_new_line.second == '\r')
    ss << "\r\n";
  else
    ss << "\n";

  return ss.str();
}
}  // namespace compress