include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

include_directories(${CMAKE_SOURCE_DIR}/src/Logger/include)
add_executable(LoggerUnitTests LoggerTests.cpp)

target_link_libraries(LoggerUnitTests
 PRIVATE
  GTest::GTest
  logger)

add_test(logger_gtests LoggerUnitTests)