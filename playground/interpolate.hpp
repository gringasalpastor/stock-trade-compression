#pragma once

#include <vector>
#include <boost/range/irange.hpp>

namespace exercise
{
    namespace interpolate
    {
        /*
        * Uses Newton's Divided-Difference algorithm to interpolate `x_interpolate` for the given input
        * ranges `x_values` and `y_values`. See Numerical Analysis(Burden & Faires) page 121
        */
        template<class XRange, class YRange>
        double newton_interpolate(const XRange& x_values, const YRange& y_values, const double x_interpolate)
        {
            std::vector<double> x(boost::begin(x_values), boost::end(x_values));
            std::vector<double> y(boost::begin(y_values), boost::end(y_values));
            
            assert(x.size() == y.size());
            assert(x.size() >= 2);
            
            const int size = x.size();
            
            std::vector<double> coefficients;
            coefficients.push_back(y[0]);
            std::vector<std::vector<double>> differences;
            differences.push_back(y);
            
            //Calculate the interpolation coefficients
            for(const auto i __attribute__((unused)) : boost::irange(1, size))
            {
                std::vector<double> diffs;
                for(const auto j : boost::irange(1, static_cast<int>(differences.back().size())))
                {
                    const auto back_diff = differences.back()[j] - differences.back()[j - 1];
                    const auto x_diff =  x[j + (size - differences.back().size())] - x[j-1];
                    diffs.push_back(back_diff/x_diff);
                }
                differences.push_back(diffs);
                coefficients.push_back(diffs.front());
            }
            
            //Calculate the sum of the products; See Numerical Analysis(Burden & Faires) as reference
            double sum = 0;
            for(const auto i : boost::irange(0, size))
            {
                double product = 1;
                for(int j = 0; j < i  ; j++)
                {
                    product *= x_interpolate - x[j];
                }
                sum += coefficients[i] * product;
            }
            
            return sum;
        }
        
        template<class YRange>
        double predict_next_value(const YRange& y_values)
        {
            auto size = boost::distance(y_values);
            //If only one value predict that value, else interpolate with x values as 0,1,2,3,4...
            if(boost::distance(y_values) == 1)
                return *boost::begin(y_values);
            else
            {
                return newton_interpolate(boost::irange(0, static_cast<int>(size)), y_values, size);
            }
        }
    }
}