#define BOOST_TEST_MODULE exercise
#include <boost/test/included/unit_test.hpp>

#include "interpolate.hpp"


BOOST_AUTO_TEST_SUITE( stats )

BOOST_AUTO_TEST_CASE( test_interpolate )
{
    
    // Example taken from Numerical Analysis(Burden & Faires)
    double value = exercise::interpolate::newton_interpolate(std::vector<double>({1.0, 1.3, 1.6, 1.9, 2.2}),
                                         std::vector<double>({.7651977, .6200860, .4554022, .2818186, .1103623}),
                                         1.5);
    
    BOOST_CHECK_CLOSE( value, .5118200, 0.01 );
    
    
    
}

BOOST_AUTO_TEST_SUITE_END()
