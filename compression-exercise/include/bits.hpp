#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <boost/noncopyable.hpp>

#include <bitset>


namespace exercise
{
    /*
     * bits_to_bytes_converter takes in multiple sequences of bits and packs them into a bytes.
     * bytes are stored in a `std::vector<uint8_t>`. 
     */
    struct bits_to_bytes_converter : boost::noncopyable
    {
        bits_to_bytes_converter() : bit_index(0)
        {}
        
        /*
         * Pack all of the bits in `bits` into a vector of bytes. The final byte is padded out with 0's
         * if the number of bits % 8 != 0
         */
        void append_bits(const std::vector<bool>& bits)
        {
            for(std::size_t i = 0; i < bits.size(); i++)
            {
                //Index into what byte we are packing this into
                unsigned long byte_index = bit_index / 8;
                //Index to the bit that we are working with
                int local_bit_index = bit_index % 8;
                
                //Append new byte if needed
                if(byte_index >= bytes.size())
                    bytes.push_back(0);
                assert(byte_index == bytes.size() - 1);
                
                
                uint8_t bit = 0;
                if(bits[i])
                    bit = 1;
                //append single bit into the current byte
                bit <<= (7 - local_bit_index);
                bytes[byte_index] |= bit;
                bit_index++;
            }
        }
        
        
        /*
         * Removes all fully packed bytes. If the final byte is not fully packed it will still remain, and
         * data().size() == 1, else data().size() == 0.
         */
        void clear_fully_packed_bytes()
        {
            bit_index %= 8;
            if(number_of_fully_packed_bytes() == bytes.size())
            {
                bytes.clear();
            }
            else
            {
                uint8_t final_byte = bytes.back();
                bytes.clear();
                bytes.push_back(final_byte);
            }
            
        }
        
        /*
         * Returns the number of fully packed bytes
         */
        unsigned long number_of_fully_packed_bytes()
        {
            return (bit_index / 8);
        }
        
        /*
         * Access to a vector of all of the bytes being stored. The final byte might not be fully packed.
         */
        const std::vector<uint8_t>& data() const
        {
            return bytes;
        }
    private:
        unsigned long bit_index;
        std::vector<uint8_t> bytes;
    };
    
    std::shared_ptr<bits_to_bytes_converter> make_bits_to_bytes_converter()
    {
        return std::make_shared<bits_to_bytes_converter>();
    }
    
    void increment_bits(std::vector<bool>& bits)
    {
        int index = bits.size()-1;
        bool carry = true;
        while(index >= 0)
        {
            //current bit is 0
            if(!bits[index])
            {
                bits[index] = true;
                carry = false;
                break;
            }
            
            //current bit is 1
            if(bits[index])
            {
                bits[index] = false;
            }
            
            index--;
        }
        
        //If all 1' add 1 to the front
        if(carry)
            bits.insert(boost::begin(bits), true);
    }

}