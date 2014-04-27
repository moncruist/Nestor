cmake_minimum_required(VERSION 2.8)
project(sqlite)
set(SQLITE3_SOURCE_LIB sqlite3.c)
add_library(sqlite3 STATIC ${SQLITE3_SOURCE_LIB})
set_target_properties(sqlite3 PROPERTIES
  COMPILE_FLAGS "-DSQLITE_ENABLE_ICU"
)
