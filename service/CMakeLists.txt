cmake_minimum_required(VERSION 2.8)

set(NESTOR_SERVICE_SOURCE 
             service.cpp
             service.h
             configuration.cpp
             configuration.h
)
             
add_library (nestorservice ${NESTOR_SERVICE_SOURCE})