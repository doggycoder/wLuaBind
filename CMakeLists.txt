cmake_minimum_required(VERSION 3.10.0)
project(wLuaBind)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_STANDARD 11)

add_subdirectory(binder luabinder.out)
include_directories(lua-5.3.5/src)
aux_source_directory(. SRC_MAIN)
aux_source_directory(./test SRC_TEST)
add_executable(${PROJECT_NAME} ${SRC_TEST} ${SRC_MAIN})
target_link_libraries(${PROJECT_NAME} lua)