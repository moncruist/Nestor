cmake_minimum_required(VERSION 2.8)
project(nestor) 

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/")

set(NESTOR_LIB_LIST logger.cpp
             utils/string.cpp
             utils/string.h
             net/http_client.cpp
             net/http_client.h
             net/socket_single.cpp
             net/socket_single.h
             net/socket_listener.cpp
             net/socket_listener.h
             net/http_resource.cpp
             net/http_resource.h
             net/http_response_parser.cpp
             net/http_response_parser.h
             net/io_observer.cpp
             net/io_observer.h
             rss/rss_object.cpp
             rss/rss_object.h
             rss/rss_channel.cpp
             rss/rss_channel.h
             rss/rss_xml_parser.cpp
             rss/rss_xml_parser.h
             imap/imap_session.cpp
             imap/imap_session.h
             imap/imap_string.cpp
             imap/imap_string.h)
include_directories(${nestor_SOURCE_DIR})
include_directories(${nestor_SOURCE_DIR}/include)

find_package(ICU REQUIRED)
find_package(Log4cplus REQUIRED)

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

find_package(Log4cplus REQUIRED)
if(NOT LOG4CPLUS_FOUND)
	message(SEND_ERROR "Cannot find Log4cplus library")
	return()
else()
	include_directories(${LOG4CPLUS_INCLUDE_DIRS})
endif()

set(NESTOR_LIB_LINKS  ${ICU_LIBRARIES}
	                  ${ICU_DATA_LIBRARIES}
	                  ${ICU_IO_LIBRARIES}
	                  ${ICU_I18N_LIBRARIES}
	                  ${TINYXML2_LIBRARY}
	                  ${LOG4CPLUS_LIBRARIES})

if(UNIX)
	add_definitions(-DUNIX)
else()
	add_definitions(-DWIN32)
endif(UNIX)



add_library (nestorcom ${NESTOR_LIB_LIST})

add_subdirectory(tests)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} nestorcom
                                      ${NESTOR_LIB_LINKS})

enable_testing()                                      
add_test (NAME "nestortests" COMMAND nestor_tests)