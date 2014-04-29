cmake_minimum_required(VERSION 2.8)

set(NESTOR_RSS_SOURCE 
             rss_object.cpp
             rss_object.h
             rss_channel.cpp
             rss_channel.h
             rss_xml_parser.cpp
             rss_xml_parser.h
)
             
add_library (nestorrss ${NESTOR_RSS_SOURCE})