cmake_minimum_required(VERSION 3.1)
project(LOB)

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type provided, default to RELEASE.")
  set(CMAKE_BUILD_TYPE "RELEASE")
endif()

option(LOB_TEST      "Build tests" OFF)
option(LOB_BENCH     "Build benchmarks" OFF)

set(LOB_LIB_NAME LOB)
set(LOB_LIB_TYPE SHARED)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(Threads REQUIRED)
find_package(Boost)
find_package(Math)

include_directories(. include ${Boost_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#if(NEMO_SANITIZE)
#  set(CMAKE_CXX_FLAGS "-fsanitize=address -fsanitize=undefined ${CMAKE_CXX_FLAGS}")
#endif()
set(CMAKE_CXX_FLAGS "-fsanitize=address -fsanitize=undefined ${CMAKE_CXX_FLAGS}")

add_subdirectory(src)

if(LOB_TEST)
  enable_testing()
  add_subdirectory(test)
endif()

if(LOB_BENCH)
  add_subdirectory(bench)
endif()

message(STATUS "")
message(STATUS "BUILD SUMMARY")
message(STATUS "  CMAKE_GENERATOR      : ${CMAKE_GENERATOR}")
message(STATUS "  Compiler ID          : ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  Build type           : ${CMAKE_BUILD_TYPE}")
message(STATUS "  Library name         : ${LOB_LIB_NAME}")
message(STATUS "  Library type         : ${LOB_LIB_TYPE}")
message(STATUS "  Build tests          : ${LOB_TEST}")
message(STATUS "  Build benchmarks     : ${LOB_BENCH}")
message(STATUS "  Boost include dirs   : ${Boost_INCLUDE_DIRS}")
message(STATUS "")
