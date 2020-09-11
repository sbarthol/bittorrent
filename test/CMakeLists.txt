set(BINARY ${CMAKE_PROJECT_NAME}_tst)

file(GLOB_RECURSE TEST_SOURCES *.cpp)

set(TEST_SOURCES ${TEST_SOURCES})

foreach (test_source ${TEST_SOURCES})
	
    get_filename_component(test_filename ${test_source} NAME)
    string(REPLACE ".cpp" "" test_name ${test_filename})

    message(WARNING "${test_source}, ${test_filename}, ${test_name}")

    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name} gtest gmock_main ${CMAKE_PROJECT_NAME}_lib)
    

endforeach(test_source ${TEST_SOURCES})