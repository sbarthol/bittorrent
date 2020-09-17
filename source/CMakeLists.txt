set(BINARY ${CMAKE_PROJECT_NAME})

file(GLOB_RECURSE SOURCES *.cpp)

set(SOURCES ${SOURCES})

add_executable(${BINARY} ${SOURCES})

target_link_libraries(${BINARY} -lcrypto -lpthread)

add_library(${BINARY}_lib STATIC ${SOURCES})

target_link_libraries(${BINARY}_lib -lcrypto -lpthread)