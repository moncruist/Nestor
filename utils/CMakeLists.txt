cmake_minimum_required(VERSION 2.8)

set(NESTOR_UTILS_SOURCE 
             string.cpp
             string.h
)
             
add_library (nestorutils ${NESTOR_UTILS_SOURCE})