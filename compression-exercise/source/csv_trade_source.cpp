#include "csv_trade_source.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace exercise
{
    trade parse_trade(const std::string& line, const char new_line)
    {
        trade t;
        std::vector<std::string> tokens;
        boost::split(tokens, line, boost::is_any_of(","));
        t.symbol    = tokens[0];
        t.exchange_new_line.first  = tokens[1][0];
        t.exchange_new_line.second  = new_line;
        t.side      = tokens[2][0];
        t.condition = tokens[3][0];
        t.send_time = boost::lexical_cast<int64_t>(tokens[4]);
        t.receive_time = boost::lexical_cast<int64_t>(tokens[5]);
        const std::string price_token = tokens[6];
        std::string price_string;
        
        unsigned p_index = 0;
        while(p_index < price_token.size() && price_token[p_index] != '.')
            price_string.push_back(price_token[p_index++]);
        
        p_index++; //Skip the '.'
        //Read the next 2 digits if they are available 
        if(p_index < price_token.size())
            price_string.push_back(price_token[p_index++]);
        else
            price_string.push_back('0');
        
        if(p_index < price_token.size())
            price_string.push_back(price_token[p_index++]);
        else
            price_string.push_back('0');
        
        t.price = std::abs(boost::lexical_cast<int32_t>(price_string));
        
        t.overflow_price_digits = "";
        if(price_token[0] == '-')
            t.overflow_price_digits.push_back('-');
        /*
         * Copy all of the extra digits to overflow_price_digits
         * Also stores the '-'
         * Example "-3.1415" --> "-15"
         */
        auto index = price_token.find('.');
        if(index != std::string::npos)
        {
            index += 3;
            if(index < price_token.size())
                t.overflow_price_digits += price_token.substr(index);
        }
        t.quantity = boost::lexical_cast<int32_t>(tokens[7]);
        return t;
    }

    boost::iterator_range<detail::csv_source_iterator> make_csv_source_range(const boost::filesystem::path& path)
    {
        return boost::make_iterator_range(
            detail::csv_source_iterator(std::make_shared<std::ifstream>(path.string(), std::ios_base::in |std::ios_base::binary)),
            detail::csv_source_iterator());
    }
    
    namespace detail 
    {
        csv_source_iterator::csv_source_iterator(std::shared_ptr<std::ifstream> stream_)
        : stream(stream_) 
        {
            parse_line();
        }

        csv_source_iterator::csv_source_iterator()
        {}

        void csv_source_iterator::parse_line()
        {
            if (std::getline(*stream, line, '\n')) 
            {
                line_ending ='\n';
                if(!line.empty() && line.back() =='\r')
                    line_ending = '\r';
                boost::trim(line);
                if(line.empty())
                    parse_line();
            }
            else
            {
                this->stream = nullptr;
            }
        }

        void csv_source_iterator::increment() 
        { 
            parse_line();
        }

        bool csv_source_iterator::equal(csv_source_iterator const& other) const
        {
            return this->stream == nullptr && other.stream == nullptr;
        }

        trade csv_source_iterator::dereference() const 
        { 
            return parse_trade(line, line_ending);
        }
    }

}