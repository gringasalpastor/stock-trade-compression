add_executable(test_csv_trade_source ./test_csv_trade_source.cpp)
TARGET_LINK_LIBRARIES(test_csv_trade_source ${Boost_LIBRARIES} trade_processing)
add_test(test_csv_trade_source test_csv_trade_source)

add_executable(test_encoder ./test_encoder.cpp)
TARGET_LINK_LIBRARIES(test_encoder ${Boost_LIBRARIES})
add_test(test_encoder test_encoder)

add_executable(test_huffman ./test_huffman.cpp)
TARGET_LINK_LIBRARIES(test_huffman ${Boost_LIBRARIES})
add_test(test_huffman test_huffman)

add_executable(test_bits ./test_bits.cpp)
TARGET_LINK_LIBRARIES(test_bits ${Boost_LIBRARIES})
add_test(test_bits test_bits)

add_executable(test_trade ./test_trade.cpp)
TARGET_LINK_LIBRARIES(test_trade ${Boost_LIBRARIES} trade_processing)
add_test(test_trade test_trade)
