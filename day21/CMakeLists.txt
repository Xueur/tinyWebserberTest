
cmake_minimum_required(VERSION 3.10)
project(tinyWebServer)


include_directories(
    base
    tcp
    http
    timer
    log
)

SET(CMAKE_CXX_FLAGS "-g -Wall -Werror -std=c++17")
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")

find_package(Threads REQUIRED)

aux_source_directory(base SRC_LIST1)
aux_source_directory(tcp SRC_LIST2)
aux_source_directory(http SRC_LIST3)
aux_source_directory(timer SRC_LIST4)
aux_source_directory(log SRC_LIST5)


set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/test/)

# add_executable(webserver test/HttpServer.cpp ${SRC_LIST1} ${SRC_LIST2} ${SRC_LIST3} ${SRC_LIST4})
add_executable(testLog test/LogTest.cpp ${SRC_LIST5})
target_link_libraries(testLog ${CMAKE_THREAD_LIBS_INIT})