cmake_minimum_required(VERSION 3.15)
project(bpe)

set(CMAKE_CXX_STANDARD 11)

find_package( Threads )
add_executable(bpe main.cpp ../../src/bpe.cpp ../../src/utf8.cpp ../../src/utils.cpp)
target_link_libraries( bpe ${CMAKE_THREAD_LIBS_INIT} )
# add_compile_options("-pthread")