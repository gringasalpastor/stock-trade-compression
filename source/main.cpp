#include <boost/exception/all.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <fstream>

#include "csv_trade_source.hpp"
#include "decoder.hpp"
#include "encoder.hpp"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, const char* argv[]) {
  try {
    boost::filesystem::path input_path;
    boost::filesystem::path output_path;
    po::options_description desc("Allowed options");
    // clang-format off
    desc.add_options()
      ("help,h",        "help")
      ("decompress,d",  "decompress the input file, compress if not specified")
      ("input-path,i",  po::value<fs::path>(&input_path)->required(),  "Input path (required)")
      ("output-path,o", po::value<fs::path>(&output_path)->required(), "Output path (required)");
    // clang-format on

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
      std::cout << desc << std::endl;
      return 0;  // success this is not an error
    }
    po::notify(vm);

    if (file_size(input_path) == 0) {
      std::ofstream s(output_path.string(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
      s.close();
      exit(0);
    }

    const bool compress = [&]() { return not vm.count("decompress"); }();

    std::ofstream fout(output_path.string(), std::ios::out);

    if (compress) {
      // Encode the data
      compress::encoder encoder(input_path, output_path);
      const auto        header_size = encoder.encode_header();
      encoder.encode_trades();

      const auto symbol_header_size           = encoder.get_symbol_header_size();
      const auto exchange_newline_header_size = encoder.get_exchange_newline_header_size();
      const auto side_header_size             = encoder.get_side_header_size();
      const auto condition_header_size        = encoder.get_condition_header_size();
      const auto send_time_header_size        = encoder.get_send_time_header_size();
      const auto receive_time_header_size     = encoder.get_receive_time_diff_header_size();
      const auto price_header_size            = encoder.get_price_header_size();
      const auto price_overflow_header_size   = encoder.get_price_overflow_header_size();
      const auto quantity_header_size         = encoder.get_quantity_header_size();

      // Print compress metrics
      const auto input_size      = boost::filesystem::file_size(input_path);
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
      const auto compression_without_header =
          100.0 - (100.0 * (static_cast<double>(compressed_size - header_size) / input_size));
      std::cout << "Compression: " << compression << "%" << std::endl;
      std::cout << "Compression (excluding header): " << compression_without_header << "%" << std::endl;

    } else {
      compress::decoder decoder(
          std::make_shared<std::ifstream>(input_path.string(), std::ios_base::in | std::ios_base::binary),
          std::make_shared<std::ofstream>(output_path.string(),
                                          std::ios_base::out | std::ios_base::binary | std::ios_base::trunc));
      decoder.decode();
    }

    return 0;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  } catch (boost::exception& e) {
    std::cerr << boost::diagnostic_information(e) << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "\nError: Caught an unknown exception!" << std::endl;
    return 1;
  }
}
