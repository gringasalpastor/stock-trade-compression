#define BOOST_TEST_MODULE exercise
#include <boost/test/included/unit_test.hpp>
#include "trade.hpp"

BOOST_AUTO_TEST_SUITE( parse )

BOOST_AUTO_TEST_CASE( trade )
{
    auto t = exercise::make_trade("GEM8-GEU8", std::make_pair('F','\n'), 'A', '0', 60303042, 60303043, 14, "-", 115);
    BOOST_REQUIRE_EQUAL("GEM8-GEU8,F,A,0,60303042,60303043,-0.14,115\n", t.to_csv());
    
    t = exercise::make_trade("GEM8-GEU8", std::make_pair('F','\n'), 'A', '0', 60303042, 60303043, 4, "-", 115);
    BOOST_REQUIRE_EQUAL("GEM8-GEU8,F,A,0,60303042,60303043,-0.04,115\n", t.to_csv());
    
    t = exercise::make_trade("GEM8-GEU8", std::make_pair('F','\n'), 'A', '0', 60303042, 60303043, 4, "-1", 115);
    BOOST_REQUIRE_EQUAL("GEM8-GEU8,F,A,0,60303042,60303043,-0.041,115\n", t.to_csv());
    
    t = exercise::make_trade("6AH8", std::make_pair('F','\n'), 'B', '0', 60306043, 60306043, 90510, "", 10);
    BOOST_REQUIRE_EQUAL("6AH8,F,B,0,60306043,60306043,905.1,10\n", t.to_csv());
    
    t = exercise::make_trade("6AH8", std::make_pair('F','\n'), 'B', '0', 60306043, 60306043, 90510, "1", 10);
    BOOST_REQUIRE_EQUAL("6AH8,F,B,0,60306043,60306043,905.101,10\n", t.to_csv());
    
    t = exercise::make_trade("6AH8", std::make_pair('F','\n'), 'B', '0', 60306043, 60306043, 90500, "1", 10);
    BOOST_REQUIRE_EQUAL("6AH8,F,B,0,60306043,60306043,905.001,10\n", t.to_csv());
    
    t = exercise::make_trade("6AH8", std::make_pair('F','\n'), 'B', '0', 60306043, 60306043, 0, "1", 10);
    BOOST_REQUIRE_EQUAL("6AH8,F,B,0,60306043,60306043,0.001,10\n", t.to_csv());
    
    t = exercise::make_trade("6AH8", std::make_pair('F','\n'), 'B', '0', 60315666, 60315666, 314, "15", 20);
    BOOST_REQUIRE_EQUAL("6AH8,F,B,0,60315666,60315666,3.1415,20\n", t.to_csv());
    
    t = exercise::make_trade("6AH8", std::make_pair('F','\r'), 'B', '0', 60315666, 60315666, 314, "15", 20);
    BOOST_REQUIRE_EQUAL("6AH8,F,B,0,60315666,60315666,3.1415,20\r\n", t.to_csv());
    
    t = exercise::make_trade("6AH8", std::make_pair('F','\r'), 'B', '0', 60315666, 60315666, 135100, "", 20);
    BOOST_REQUIRE_EQUAL("6AH8,F,B,0,60315666,60315666,1351,20\r\n", t.to_csv());
}

BOOST_AUTO_TEST_SUITE_END()
