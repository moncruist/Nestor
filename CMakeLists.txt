cmake_minimum_required(VERSION 2.8)
project(nestor) 

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

set(SRC_LIST main.cpp
			 net/http_client.cpp)

add_executable(${PROJECT_NAME} ${SRC_LIST})