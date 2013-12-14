cmake_minimum_required(VERSION 2.8)
project(nestor) 

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/")

set(SRC_LIST main.cpp
			 net/http_client.cpp)

find_package(ICU REQUIRED)
if(NOT ICU_FOUND)
	message(SEND_ERROR "Cannot find icu library")
	return()
else()
	include_directories(${ICU_INCLUDE_DIRS})
endif()

find_package(TinyXML2 REQUIRED)
if(NOT TINYXML2_FOUND)
	message(SEND_ERROR "Cannot find TinyXML2 library")
	return()
else()
	include_directories(${TINYXML2_INCLUDE_DIR})
endif()

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${ICU_LIBRARIES}
									  ${TINYXML2_LIBRARY})