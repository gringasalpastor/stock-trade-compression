#define BOOST_TEST_MODULE exercise
#include <boost/test/included/unit_test.hpp>
#include "csv_trade_source.hpp"

BOOST_AUTO_TEST_SUITE( parse )

BOOST_AUTO_TEST_CASE( parse_trade )
{
    auto t = exercise::parse_trade("GEM8-GEU8,F,A,0,60303042,60303043,-0.14,115", '\n');

    BOOST_REQUIRE_EQUAL(t.symbol, "GEM8-GEU8");
    BOOST_REQUIRE_EQUAL(t.exchange_new_line.first, 'F');
    BOOST_REQUIRE_EQUAL(t.exchange_new_line.second, '\n');
    BOOST_REQUIRE_EQUAL(t.side, 'A');
    BOOST_REQUIRE_EQUAL(t.condition, '0');
    BOOST_REQUIRE_EQUAL(t.send_time, 60303042);
    BOOST_REQUIRE_EQUAL(t.receive_time, 60303043);
    BOOST_REQUIRE_EQUAL(t.price, 14);
    BOOST_REQUIRE_EQUAL(t.overflow_price_digits, "-");
    BOOST_REQUIRE_EQUAL(t.quantity, 115);
    
    t = exercise::parse_trade("GEM8-GEU8,F,A,0,60303042,60303043,-3.1415,115", '\r');

    BOOST_REQUIRE_EQUAL(t.symbol, "GEM8-GEU8");
    BOOST_REQUIRE_EQUAL(t.exchange_new_line.first, 'F');
    BOOST_REQUIRE_EQUAL(t.exchange_new_line.second, '\r');
    BOOST_REQUIRE_EQUAL(t.side, 'A');
    BOOST_REQUIRE_EQUAL(t.condition, '0');
    BOOST_REQUIRE_EQUAL(t.send_time, 60303042);
    BOOST_REQUIRE_EQUAL(t.receive_time, 60303043);
    BOOST_REQUIRE_EQUAL(t.price, 314);
    BOOST_REQUIRE_EQUAL(t.overflow_price_digits, "-15");
    BOOST_REQUIRE_EQUAL(t.quantity, 115);
    
    t = exercise::parse_trade("GEM8-GEU8,F,A,0,60303042,60303043,-0.1415,115", '\r');

    BOOST_REQUIRE_EQUAL(t.symbol, "GEM8-GEU8");
    BOOST_REQUIRE_EQUAL(t.exchange_new_line.first, 'F');
    BOOST_REQUIRE_EQUAL(t.exchange_new_line.second, '\r');
    BOOST_REQUIRE_EQUAL(t.side, 'A');
    BOOST_REQUIRE_EQUAL(t.condition, '0');
    BOOST_REQUIRE_EQUAL(t.send_time, 60303042);
    BOOST_REQUIRE_EQUAL(t.receive_time, 60303043);
    BOOST_REQUIRE_EQUAL(t.price, 14);
    BOOST_REQUIRE_EQUAL(t.overflow_price_digits, "-15");
    BOOST_REQUIRE_EQUAL(t.quantity, 115);
    
    t = exercise::parse_trade("GEM8-GEU8,F,A,0,60303042,60303043,-14,115", '\r');

    BOOST_REQUIRE_EQUAL(t.symbol, "GEM8-GEU8");
    BOOST_REQUIRE_EQUAL(t.exchange_new_line.first, 'F');
    BOOST_REQUIRE_EQUAL(t.exchange_new_line.second, '\r');
    BOOST_REQUIRE_EQUAL(t.side, 'A');
    BOOST_REQUIRE_EQUAL(t.condition, '0');
    BOOST_REQUIRE_EQUAL(t.send_time, 60303042);
    BOOST_REQUIRE_EQUAL(t.receive_time, 60303043);
    BOOST_REQUIRE_EQUAL(t.price, 1400);
    BOOST_REQUIRE_EQUAL(t.overflow_price_digits, "-");
    BOOST_REQUIRE_EQUAL(t.quantity, 115);
    
    t = exercise::parse_trade("GEM8-GEU8,F,A,0,60303042,60303043,89.5,115", '\r');

    BOOST_REQUIRE_EQUAL(t.symbol, "GEM8-GEU8");
    BOOST_REQUIRE_EQUAL(t.exchange_new_line.first, 'F');
    BOOST_REQUIRE_EQUAL(t.exchange_new_line.second, '\r');
    BOOST_REQUIRE_EQUAL(t.side, 'A');
    BOOST_REQUIRE_EQUAL(t.condition, '0');
    BOOST_REQUIRE_EQUAL(t.send_time, 60303042);
    BOOST_REQUIRE_EQUAL(t.receive_time, 60303043);
    BOOST_REQUIRE_EQUAL(t.price, 8950);
    BOOST_REQUIRE_EQUAL(t.overflow_price_digits, "");
    BOOST_REQUIRE_EQUAL(t.quantity, 115);
    
    t = exercise::parse_trade("GEM8-GEU8,F,A,0,60303042,60303043,1351,115", '\r');

    BOOST_REQUIRE_EQUAL(t.symbol, "GEM8-GEU8");
    BOOST_REQUIRE_EQUAL(t.exchange_new_line.first, 'F');
    BOOST_REQUIRE_EQUAL(t.exchange_new_line.second, '\r');
    BOOST_REQUIRE_EQUAL(t.side, 'A');
    BOOST_REQUIRE_EQUAL(t.condition, '0');
    BOOST_REQUIRE_EQUAL(t.send_time, 60303042);
    BOOST_REQUIRE_EQUAL(t.receive_time, 60303043);
    BOOST_REQUIRE_EQUAL(t.price, 135100);
    BOOST_REQUIRE_EQUAL(t.overflow_price_digits, "");
    BOOST_REQUIRE_EQUAL(t.quantity, 115);
    
    t = exercise::parse_trade("GEM8-GEU8,F,A,0,60303042,60303043,-0.005,115", '\r');

    BOOST_REQUIRE_EQUAL(t.symbol, "GEM8-GEU8");
    BOOST_REQUIRE_EQUAL(t.exchange_new_line.first, 'F');
    BOOST_REQUIRE_EQUAL(t.exchange_new_line.second, '\r');
    BOOST_REQUIRE_EQUAL(t.side, 'A');
    BOOST_REQUIRE_EQUAL(t.condition, '0');
    BOOST_REQUIRE_EQUAL(t.send_time, 60303042);
    BOOST_REQUIRE_EQUAL(t.receive_time, 60303043);
    BOOST_REQUIRE_EQUAL(t.price, 0);
    BOOST_REQUIRE_EQUAL(t.overflow_price_digits, "-5");
    BOOST_REQUIRE_EQUAL(t.quantity, 115);
    
    t = exercise::parse_trade("GEM8-GEU8,F,A,0,60303042,60303043,-1,115", '\r');

    BOOST_REQUIRE_EQUAL(t.symbol, "GEM8-GEU8");
    BOOST_REQUIRE_EQUAL(t.exchange_new_line.first, 'F');
    BOOST_REQUIRE_EQUAL(t.exchange_new_line.second, '\r');
    BOOST_REQUIRE_EQUAL(t.side, 'A');
    BOOST_REQUIRE_EQUAL(t.condition, '0');
    BOOST_REQUIRE_EQUAL(t.send_time, 60303042);
    BOOST_REQUIRE_EQUAL(t.receive_time, 60303043);
    BOOST_REQUIRE_EQUAL(t.price, 100);
    BOOST_REQUIRE_EQUAL(t.overflow_price_digits, "-");
    BOOST_REQUIRE_EQUAL(t.quantity, 115);
    
    t = exercise::parse_trade("GEM8-GEU8,F,A,0,60303042,60303043,-1.1,115", '\r');

    BOOST_REQUIRE_EQUAL(t.symbol, "GEM8-GEU8");
    BOOST_REQUIRE_EQUAL(t.exchange_new_line.first, 'F');
    BOOST_REQUIRE_EQUAL(t.exchange_new_line.second, '\r');
    BOOST_REQUIRE_EQUAL(t.side, 'A');
    BOOST_REQUIRE_EQUAL(t.condition, '0');
    BOOST_REQUIRE_EQUAL(t.send_time, 60303042);
    BOOST_REQUIRE_EQUAL(t.receive_time, 60303043);
    BOOST_REQUIRE_EQUAL(t.price, 110);
    BOOST_REQUIRE_EQUAL(t.overflow_price_digits, "-");
    BOOST_REQUIRE_EQUAL(t.quantity, 115);
    
    t = exercise::parse_trade("GEM8-GEU8,F,A,0,60303042,60303043,-1.01,115", '\r');

    BOOST_REQUIRE_EQUAL(t.symbol, "GEM8-GEU8");
    BOOST_REQUIRE_EQUAL(t.exchange_new_line.first, 'F');
    BOOST_REQUIRE_EQUAL(t.exchange_new_line.second, '\r');
    BOOST_REQUIRE_EQUAL(t.side, 'A');
    BOOST_REQUIRE_EQUAL(t.condition, '0');
    BOOST_REQUIRE_EQUAL(t.send_time, 60303042);
    BOOST_REQUIRE_EQUAL(t.receive_time, 60303043);
    BOOST_REQUIRE_EQUAL(t.price, 101);
    BOOST_REQUIRE_EQUAL(t.overflow_price_digits, "-");
    BOOST_REQUIRE_EQUAL(t.quantity, 115);
    
    t = exercise::parse_trade("GE:BF Z8-H9-M9,F,A,0,60303042,60303043,-1.01,115", '\r');

    BOOST_REQUIRE_EQUAL(t.symbol, "GE:BF Z8-H9-M9");
    BOOST_REQUIRE_EQUAL(t.exchange_new_line.first, 'F');
    BOOST_REQUIRE_EQUAL(t.exchange_new_line.second, '\r');
    BOOST_REQUIRE_EQUAL(t.side, 'A');
    BOOST_REQUIRE_EQUAL(t.condition, '0');
    BOOST_REQUIRE_EQUAL(t.send_time, 60303042);
    BOOST_REQUIRE_EQUAL(t.receive_time, 60303043);
    BOOST_REQUIRE_EQUAL(t.price, 101);
    BOOST_REQUIRE_EQUAL(t.overflow_price_digits, "-");
    BOOST_REQUIRE_EQUAL(t.quantity, 115);
    
}

BOOST_AUTO_TEST_SUITE_END()
