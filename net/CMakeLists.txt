cmake_minimum_required(VERSION 2.8)

set(NESTOR_NET_SOURCE 
             http_client.cpp
             http_client.h
             socket_single.cpp
             socket_single.h
             socket_listener.cpp
             socket_listener.h
             http_resource.cpp
             http_resource.h
             http_response_parser.cpp
             http_response_parser.h
             io_observer.cpp
             io_observer.h
)
             
add_library (nestornet ${NESTOR_NET_SOURCE})