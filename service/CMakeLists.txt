cmake_minimum_required(VERSION 2.8)

set(NESTOR_SERVICE_SOURCE 
             service.cpp
             service.h
             configuration.cpp
             configuration.h
             sqlite_provider.cpp
             sqlite_provider.h
             sqlite_connection.cpp
             sqlite_connection.h
             types.cpp
             types.h
)
             
add_library (nestorservice ${NESTOR_SERVICE_SOURCE})