#define BOOST_TEST_MODULE compress

#include <boost/test/included/unit_test.hpp>
#include <boost/utility/binary.hpp>

#include "bits.hpp"

BOOST_AUTO_TEST_SUITE(stats)

BOOST_AUTO_TEST_CASE(bits_to_bytes_converter) {
  auto converter = compress::make_bits_to_bytes_converter();

  std::vector<bool> bits;
  bits.push_back(true);
  bits.push_back(true);
  bits.push_back(false);

  converter->append_bits(bits);
  BOOST_REQUIRE_EQUAL(converter->data()[0], BOOST_BINARY(11000000));
  BOOST_REQUIRE_EQUAL(converter->data().size(), 1);

  bits.push_back(false);
  bits.push_back(true);
  bits.push_back(false);

  converter->append_bits(bits);

  BOOST_REQUIRE_EQUAL(converter->data()[0], BOOST_BINARY(11011001));
  BOOST_REQUIRE_EQUAL(converter->data()[1], BOOST_BINARY(00000000));
  BOOST_REQUIRE_EQUAL(converter->data().size(), 2);

  bits.push_back(true);
  bits.push_back(true);
  bits.push_back(true);

  converter->append_bits(bits);

  BOOST_REQUIRE_EQUAL(converter->data()[0], BOOST_BINARY(11011001));
  BOOST_REQUIRE_EQUAL(converter->data()[1], BOOST_BINARY(01100101));
  BOOST_REQUIRE_EQUAL(converter->data()[2], BOOST_BINARY(11000000));
  BOOST_REQUIRE_EQUAL(converter->data().size(), 3);
  BOOST_REQUIRE_EQUAL(converter->number_of_fully_packed_bytes(), 2);
}

BOOST_AUTO_TEST_SUITE_END()
