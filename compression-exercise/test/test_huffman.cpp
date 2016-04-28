#define BOOST_TEST_MODULE exercise
#include <boost/test/included/unit_test.hpp>

#include "huffman.hpp"

BOOST_AUTO_TEST_SUITE( stats )

BOOST_AUTO_TEST_CASE( test_populate_size_map )
{
    auto count_map = exercise::huffman::make_count_map<char>();

    //Example taken from <http://en.wikipedia.org/wiki/Huffman_coding>
    (*count_map)[' '] = 7;
    (*count_map)['a'] = 4;
    (*count_map)['e'] = 4;
    (*count_map)['f'] = 3;
    (*count_map)['h'] = 2;
    (*count_map)['i'] = 2;
    (*count_map)['m'] = 2;
    (*count_map)['n'] = 2;
    (*count_map)['s'] = 2;
    (*count_map)['t'] = 2;
    (*count_map)['l'] = 1;
    (*count_map)['o'] = 1;
    (*count_map)['p'] = 1;
    (*count_map)['r'] = 1;
    (*count_map)['u'] = 1;
    (*count_map)['x'] = 1;
        
    auto prefix_map = exercise::huffman::create_prefix_map(exercise::huffman::make_huffman_tree(count_map, 36));
    auto canonical_map = exercise::huffman::create_canonical_prefix_map(prefix_map);

    BOOST_REQUIRE_EQUAL((*prefix_map)[' '].size(), 3 );
    BOOST_REQUIRE_EQUAL((*prefix_map)['a'].size(), 3 );
    BOOST_REQUIRE_EQUAL((*prefix_map)['e'].size(), 3 );
    BOOST_REQUIRE_EQUAL((*prefix_map)['f'].size(), 4 );
    BOOST_REQUIRE_EQUAL((*prefix_map)['h'].size(), 4 );
    BOOST_REQUIRE_EQUAL((*prefix_map)['i'].size(), 4 );
    BOOST_REQUIRE_EQUAL((*prefix_map)['m'].size(), 4 );
    BOOST_REQUIRE_EQUAL((*prefix_map)['n'].size(), 4 );
    BOOST_REQUIRE_EQUAL((*prefix_map)['s'].size(), 4 );
    BOOST_REQUIRE_EQUAL((*prefix_map)['t'].size(), 4 );
    BOOST_REQUIRE_EQUAL((*prefix_map)['l'].size(), 5 );
    BOOST_REQUIRE_EQUAL((*prefix_map)['o'].size(), 5 );
    BOOST_REQUIRE_EQUAL((*prefix_map)['p'].size(), 5 );
    BOOST_REQUIRE_EQUAL((*prefix_map)['r'].size(), 5 );
    BOOST_REQUIRE_EQUAL((*prefix_map)['u'].size(), 5 );
    BOOST_REQUIRE_EQUAL((*prefix_map)['x'].size(), 5 );
    
    BOOST_REQUIRE_EQUAL((*canonical_map)[' '].size(), 3 );
    BOOST_REQUIRE_EQUAL((*canonical_map)['a'].size(), 3 );
    BOOST_REQUIRE_EQUAL((*canonical_map)['e'].size(), 3 );
    BOOST_REQUIRE_EQUAL((*canonical_map)['f'].size(), 4 );
    BOOST_REQUIRE_EQUAL((*canonical_map)['h'].size(), 4 );
    BOOST_REQUIRE_EQUAL((*canonical_map)['i'].size(), 4 );
    BOOST_REQUIRE_EQUAL((*canonical_map)['m'].size(), 4 );
    BOOST_REQUIRE_EQUAL((*canonical_map)['n'].size(), 4 );
    BOOST_REQUIRE_EQUAL((*canonical_map)['s'].size(), 4 );
    BOOST_REQUIRE_EQUAL((*canonical_map)['t'].size(), 4 );
    BOOST_REQUIRE_EQUAL((*canonical_map)['l'].size(), 5 );
    BOOST_REQUIRE_EQUAL((*canonical_map)['o'].size(), 5 );
    BOOST_REQUIRE_EQUAL((*canonical_map)['p'].size(), 5 );
    BOOST_REQUIRE_EQUAL((*canonical_map)['r'].size(), 5 );
    BOOST_REQUIRE_EQUAL((*canonical_map)['u'].size(), 5 );
    BOOST_REQUIRE_EQUAL((*canonical_map)['x'].size(), 5 );
}

BOOST_AUTO_TEST_SUITE_END()
