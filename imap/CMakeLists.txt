cmake_minimum_required(VERSION 2.8)

set(NESTOR_IMAP_SOURCE 
             imap_session.cpp
             imap_session.h
             imap_string.cpp
             imap_string.h
)
             
add_library (nestorimap ${NESTOR_IMAP_SOURCE})