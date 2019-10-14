cmake_minimum_required(VERSION 2.8)
project(Exercise)
enable_testing()

add_definitions(
    -std=c++11
    -Wall
    -Wextra
)

include_directories(./include)

find_package(Boost COMPONENTS unit_test_framework filesystem system REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )

set(SourceFiles
./source/trade.cpp
./source/csv_trade_source.cpp
)

add_library(trade_processing ${SourceFiles})

#Binary
add_executable(compress ./source/main.cpp )
TARGET_LINK_LIBRARIES(compress ${Boost_LIBRARIES} trade_processing)

#Tests
add_executable(test_csv_trade_source ./test/test_csv_trade_source.cpp)
TARGET_LINK_LIBRARIES(test_csv_trade_source ${Boost_LIBRARIES} trade_processing)
add_test(test_csv_trade_source test_csv_trade_source)

add_executable(test_encoder ./test/test_encoder.cpp)
TARGET_LINK_LIBRARIES(test_encoder ${Boost_LIBRARIES})
add_test(test_encoder test_encoder)

add_executable(test_huffman ./test/test_huffman.cpp)
TARGET_LINK_LIBRARIES(test_huffman ${Boost_LIBRARIES})
add_test(test_huffman test_huffman)

add_executable(test_bits ./test/test_bits.cpp)
TARGET_LINK_LIBRARIES(test_bits ${Boost_LIBRARIES})
add_test(test_bits test_bits)

add_executable(test_trade ./test/test_trade.cpp)
TARGET_LINK_LIBRARIES(test_trade ${Boost_LIBRARIES} trade_processing)
add_test(test_trade test_trade)