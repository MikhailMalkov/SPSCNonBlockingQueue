cmake_minimum_required(VERSION 3.17)
project (TSPSCNonBlockQueue)

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)

set(CMAKE_CXX_STANDARD 17)

add_executable(TSPSCNonBlockQueue main.cpp)