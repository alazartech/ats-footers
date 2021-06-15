cmake_minimum_required(VERSION 3.1)

project(atsfooters LANGUAGES CXX)

option(CSHARP_CODE_SAMPLE "Build a code sample in C#")

if (CSHARP_CODE_SAMPLE)
  enable_language(CSharp)
endif ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(atsfooters SHARED
  include/atsfooters.hpp
  src/atsfooters.cpp
  src/atsfooters_internal.cpp
  src/atsfooters_internal.hpp)
target_include_directories(atsfooters PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_compile_definitions(atsfooters
  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:ATSFOOTERSLIBEXPORT>
  PUBLIC
    $<$<CXX_COMPILER_ID:MSVC>:_SILENCE_CXX17_C_HEADER_DEPRECATION_WARNING>
    $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)

include(CTest)
enable_testing()

add_executable(test_atsfooters
  tests/test_atsfooters.cpp)
target_link_libraries(test_atsfooters PUBLIC atsfooters)
target_include_directories(test_atsfooters
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/tests
    ${CMAKE_CURRENT_LIST_DIR}/src)
add_test(test_atsfooters ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_atsfooters)


file(GLOB BINARY_FILES "tests/*.bin")
file(COPY ${BINARY_FILES} DESTINATION ${CMAKE_BINARY_DIR})

if (CSHARP_CODE_SAMPLE)
  add_executable(atsfooters_csharp
    test/atsfooters_csharp.cs)
  target_link_libraries(atsfooters_csharp PUBLIC atsfooters)
endif ()
