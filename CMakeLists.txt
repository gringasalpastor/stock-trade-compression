cmake_minimum_required(VERSION 3.10)
project(stock-trade-compression)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

add_definitions(-std=c++17 -Wall -Wextra -Werror)
enable_testing()

# workaround cmake issue in boost
# see: https://stackoverflow.com/questions/58081084/target-boostlibrary-already-has-an-imported-location-link-errors
set(Boost_NO_BOOST_CMAKE 1)

find_package(Boost 1.71 COMPONENTS unit_test_framework filesystem system program_options REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIR})

add_subdirectory(source)
