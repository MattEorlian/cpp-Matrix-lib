cmake_minimum_required(VERSION 3.10)

set(LIB_NAME "my_matrix_lib")

add_library(${LIB_NAME} STATIC my_matrix.cpp)

target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})