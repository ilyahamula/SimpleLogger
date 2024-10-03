set(SOURCES Logger.cpp)
add_library(logger STATIC ${SOURCES})
target_include_directories(logger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)