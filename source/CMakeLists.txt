set(BINARY ${CMAKE_PROJECT_NAME})

file(GLOB_RECURSE SOURCES *.h *.cpp)

set(SOURCES ${SOURCES})

add_executable(${BINARY}_run ${SOURCES})

target_link_libraries(${BINARY}_run -lcrypto)

add_library(${BINARY}_lib STATIC ${SOURCES})

target_link_libraries(${BINARY}_lib -lcrypto)