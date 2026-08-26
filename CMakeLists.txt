cmake_minimum_required(VERSION 3.31)

project(simple-web-server C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Werror)

file(GLOB SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.c")

add_executable(simple-web-server ${SOURCES})

target_include_directories(simple-web-server PRIVATE "${CMAKE_SOURCE_DIR}/include")
