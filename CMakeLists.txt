cmake_minimum_required(VERSION 2.8)
project(nestor) 

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/")
             
include_directories(${nestor_SOURCE_DIR})
include_directories(${nestor_SOURCE_DIR}/include)

find_package(ICU REQUIRED)
find_package(Log4cplus REQUIRED)
find_package(Threads REQUIRED)

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

find_package(LibConfig++ REQUIRED)
if(NOT LIBCONFIG++_FOUND)
	message(SEND_ERROR "Cannot find LibConfig++ library")
	return()
else()
	include_directories(${LIBCONFIG++_INCLUDE_DIR})
endif()

find_package(LibCurl REQUIRED)
if(NOT LIBCURL_FOUND)
	message(SEND_ERROR "Cannot find LibCurl library")
	return()
else()
	include_directories(${LIBCURL_INCLUDE_DIR})
endif()

find_package(LibEv REQUIRED)
if(NOT LIBEV_FOUND)
	message(SEND_ERROR "Cannot find libev library")
	return()
else()
	include_directories(${LIBEV_INCLUDE_DIR})
endif()


set(NESTOR_LIB_LINKS  sqlite3
                      dl
                      ${ICU_LIBRARIES}
	                  ${ICU_DATA_LIBRARIES}
	                  ${ICU_IO_LIBRARIES}
	                  ${ICU_I18N_LIBRARIES}
	                  ${TINYXML2_LIBRARY}
	                  ${LOG4CPLUS_LIBRARIES}
	                  ${LIBCONFIG++_LIBRARY}
                      ${LIBCURL_LIBRARY}
	                  ${CMAKE_THREAD_LIBS_INIT}
	                  ${LIBEV_LIBRARY})

if(UNIX)
	add_definitions(-DUNIX)
else()
	add_definitions(-DWIN32)
endif(UNIX)

include_directories(lib/sqlite-amalgamation-3080403)
add_subdirectory(lib/sqlite-amalgamation-3080403)

add_subdirectory(common)
add_subdirectory(net)
add_subdirectory(imap)
add_subdirectory(rss)
add_subdirectory(utils)
add_subdirectory(service)


add_subdirectory(tests)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} nestorcommon
                                      nestorservice
                                      nestornet
                                      nestorrss
                                      nestorimap
                                      nestorutils
                                      ${NESTOR_LIB_LINKS})

enable_testing()                                      
add_test (NAME "nestortests" COMMAND nestor_tests)