cmake_minimum_required(VERSION 2.8)

set(NESTOR_COMMON_SOURCE 
             logger.cpp
             logger.h
)
             
add_library (nestorcommon ${NESTOR_COMMON_SOURCE})