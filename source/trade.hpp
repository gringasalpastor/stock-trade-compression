#ifndef COMPRESS_TRADE_H
#define COMPRESS_TRADE_H

#include <boost/format.hpp>
#include <boost/range/begin.hpp>
#include <cstdint>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

namespace compress {
/*
 * Simple struct for storing a single trade.
 *
 * Most values are clear. Exceptions include:
 * `exchange_new_line` - pair of the exchange and newline char. See below
 * `price` - Stores price as price * 100 truncated to 2 decimal places ('3.1415' --> '314') `overflow_price_digits` -
 * Stores the remaining truncated decimal places from the price value examples: '3.1415' --> '15'  '3.14' --> '' (empty
 * string)
 */
struct trade {
  std::string symbol;  // Variable-length string of chars
  /*
   * Stores a pair of the newline and the exchange. The newline for a given trade is ('\n' = Unix, '\r' = Windows)
   */
  std::pair<char, char> exchange_new_line;
  char                  side;          // Type of event. Can be B/b/A/a/T
  char                  condition;     // Indicating type of quote or trade
  int64_t               send_time;     // Time at which event occurred on exchange (milliseconds-after-midnight)
  int64_t               receive_time;  // Time at which the event was received (milliseconds-after-midnight)
  int32_t               price;         // Price on the event, in USD (stored as 100 * price). Can be negative.
  std::string overflow_price_digits;   // Prices with more than 2 decimal digits(ex. 3.1415) will 'extra' digits here
  int32_t     quantity;                // Number of shares on the event

  std::string to_csv() const;
};

/*
 * Creates a trade from all of the trade data.
 */
trade make_trade(const std::string& symbol_, const std::pair<char, char>& exchange_new_line_, const char side_,
                 const char condition_, const int64_t send_time_, const int64_t receive_time_, const int32_t price_,
                 const std::string& overflow_price_digits_, const int32_t quantity_);

}  // namespace compress

#endif
