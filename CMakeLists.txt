cmake_minimum_required(VERSION 3.2) 

set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(BitTorrent
        VERSION 1.0
        DESCRIPTION "simple torrenting app"
        LANGUAGES C CXX
        )

include_directories(source)

add_subdirectory(source)
add_subdirectory(test)
add_subdirectory(googletest)



