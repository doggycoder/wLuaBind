cmake_minimum_required(VERSION 3.5)

project(lua)
include_directories(src)
aux_source_directory(src SOURCE_LIST)
add_library(${PROJECT_NAME} STATIC ${SOURCE_LIST})
