cmake_minimum_required(VERSION 3.8)
project(halite_orion)

set(CMAKE_CXX_STANDARD 11)
set(TEST 0)

include_directories(${CMAKE_SOURCE_DIR}/hlt ${CMAKE_SOURCE_DIR}/bot)


get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)

foreach (dir ${dirs})
    file(GLOB_RECURSE SOURCE ${dir}/*.[ch]*)
    set(SOURCE_FILES "${SOURCE_FILES}" ${SOURCE})
endforeach ()

include_directories(${CMAKE_SOURCE_DIR})

if (TEST)
    set(SOURCE_FILES "${SOURCE_FILES}" test.cpp)
else ()
    set(SOURCE_FILES "${SOURCE_FILES}" MyBot.cpp)
endif ()

add_executable(halite_orion ${SOURCE_FILES})