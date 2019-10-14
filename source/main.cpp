#include "csv_trade_source.hpp"
#include "encoder.hpp"
#include "decoder.hpp"
#include <boost/filesystem.hpp>
#include <fstream>

int main( int argc, const char* argv[] )
{
    if(argc != 4)
    {
        std::cerr << "Usage:" << std::endl;
        std::cerr << "compress [option] <input_file> <output_file>" << std::endl;
        return 1;
    }
    
    const std::string option = argv[1];
    const boost::filesystem::path input_path = argv[2];
    const boost::filesystem::path output_path = argv[3];

    std::ofstream fout (output_path.string(), std::ios::out);
    
    if(option == "-c")
    {
        //Encode the data
        auto encoder = exercise::make_encoder(input_path, output_path);
        const auto header_size = encoder->encode_header();
        encoder->encode_trades();
         
        const auto symbol_header_size = encoder->get_symbol_header_size();
        const auto exchange_newline_header_size = encoder->get_exchange_newline_header_size();
        const auto side_header_size = encoder->get_side_header_size();
        const auto condition_header_size = encoder->get_condition_header_size();
        const auto send_time_header_size = encoder->get_send_time_header_size();
        const auto receive_time_header_size = encoder->get_receive_time_diff_header_size();
        const auto price_header_size = encoder->get_price_header_size();
        const auto price_overflow_header_size = encoder->get_price_overflow_header_size();
        const auto quantity_header_size = encoder->get_quantity_header_size();
        encoder.reset(); //Make sure all data has been written out.
        
        //Print compress metrics
        const auto input_size = boost::filesystem::file_size(input_path);
        const auto compressed_size = boost::filesystem::file_size(output_path);
        
        std::cout << "Input Size (bytes): " << input_size << std::endl;
        std::cout << "Output Size (bytes): " << compressed_size << std::endl;
        std::cout << "\tAll Header Data: " << header_size << std::endl;
        std::cout << "\t\tSymbol Header: " << symbol_header_size << std::endl;
        std::cout << "\t\tExchange/Newline Header: " << exchange_newline_header_size << std::endl;
        std::cout << "\t\tSide Header: " << side_header_size << std::endl;
        std::cout << "\t\tCondition Header: " << condition_header_size << std::endl;
        std::cout << "\t\tSend Time Header: " << send_time_header_size << std::endl;
        std::cout << "\t\tReceive Time Header: " << receive_time_header_size << std::endl;
        std::cout << "\t\tPrice Header: " << price_header_size << std::endl;
        std::cout << "\t\tPrice Overflow Header: " << price_overflow_header_size << std::endl;
        std::cout << "\t\tQuantity Header: " << quantity_header_size << std::endl;
        
        std::cout << "\tData: " << compressed_size - header_size << std::endl;
        const auto compression = 100.0 - (100.0 * (static_cast<double>(compressed_size) / input_size));
        const auto compression_without_header = 100.0 - (100.0 * (static_cast<double>(compressed_size - header_size) / input_size));
        std::cout << "Compression: " << compression << "%" << std::endl;
        std::cout << "Compression (excluding header): " << compression_without_header << "%" << std::endl;
        
    }
    else if (option == "-d")
    {
        //If empty, make an empty file
        if(file_size(input_path) == 0)
        {
            std::ofstream(output_path.string(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
            exit(0);
        }
            
        auto decoder = exercise::make_decoder(std::make_shared<std::ifstream>(input_path.string(),
                                                                             std::ios_base::in | 
                                                                             std::ios_base::binary),
                                              std::make_shared<std::ofstream>(output_path.string(),
                                                                             std::ios_base::out | 
                                                                             std::ios_base::binary | 
                                                                             std::ios_base::trunc));
        decoder->decode();
    }
    else
    {
        std::cerr << "Unknown option: '" + option + "'" << std::endl;
    }

    return 0;
}
