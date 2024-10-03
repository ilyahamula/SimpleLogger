cmake_minimum_required(VERSION 3.0)
project(LoggerExample)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/Logger/include)
add_executable(LoggerExample main.cpp ClassExamples.cpp)

add_subdirectory(Logger)
# Link the executable with the logger library
target_link_libraries(LoggerExample logger)
