include_directories(.)
add_library(trade_processing trade.cpp csv_trade_source.cpp)

add_executable(compress main.cpp)
TARGET_LINK_LIBRARIES(compress ${Boost_LIBRARIES} trade_processing)

add_subdirectory(test)
