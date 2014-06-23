find_package(CppUnit REQUIRED)
include_directories(${CPPUNIT_INCLUDE_DIR})

add_executable(nestor_tests run.cpp
							imap_session_test.cpp
							imap_session_test.h
                            imap_string_test.cpp
                            imap_string_test.h)

#SET_TARGET_PROPERTIES(ttest PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_RUNTIME_OUTPUT_DIRECTORY}")

ADD_CUSTOM_COMMAND(TARGET tests POST_BUILD
  COMMAND "nestor_tests")
ADD_CUSTOM_TARGET(tests "nestor_tests" DEPENDS nestor_tests COMMENT "Running CPPUNIT nestor_tests...")

target_link_libraries(nestor_tests nestorcommon                                       
                                   nestornet
                                   nestorrss
                                   nestorimap
                                   nestorservice
                                   nestorutils 
                                   ${NESTOR_LIB_LINKS} 
                                   ${CPPUNIT_LIBRARY})