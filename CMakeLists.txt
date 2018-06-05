cmake_minimum_required(VERSION 3.0.0)
project(fuck)

add_subdirectory(glog)
add_subdirectory(googletest)
include_directories(.)

add_library(utils byteArrQueue.cpp)
target_link_libraries(utils glog)

add_executable(uart usrt_test.cpp)
target_link_libraries(uart utils)


add_executable(tt test/main.cpp test/byteArrQueueTest.cpp byteArrQueue.cpp)
target_link_libraries(tt gtest)
target_link_libraries(tt utils)
