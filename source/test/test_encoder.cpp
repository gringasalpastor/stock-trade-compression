#define BOOST_TEST_MODULE compress
#include <boost/test/included/unit_test.hpp>

#include "encoder.hpp"

BOOST_AUTO_TEST_SUITE(stats)

BOOST_AUTO_TEST_CASE(test_to_bytes) {
  int64_t val = 3;

  auto bytes = compress::detail::to_bytes(val);

  // This needs to take in account endianness
  BOOST_REQUIRE_EQUAL(bytes[0], 3);
  BOOST_REQUIRE_EQUAL(bytes[1], 0);
  BOOST_REQUIRE_EQUAL(bytes[2], 0);
  BOOST_REQUIRE_EQUAL(bytes[3], 0);
  BOOST_REQUIRE_EQUAL(bytes[4], 0);
  BOOST_REQUIRE_EQUAL(bytes[5], 0);
  BOOST_REQUIRE_EQUAL(bytes[6], 0);
  BOOST_REQUIRE_EQUAL(bytes[7], 0);
}

BOOST_AUTO_TEST_SUITE_END()
