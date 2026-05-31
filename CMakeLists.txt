cmake_minimum_required(VERSION 3.22)

project(meta-root)

option(META_ENABLE_TESTS "" ON)

set(CMAKE_CXX_STANDARD 26)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # MSVC compiler options
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
  # GNU compiler options
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# --- dependencies

# none

# --- library

add_subdirectory(Meta)

# --- everything else...

if(META_ENABLE_TESTS)
  add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
endif(META_ENABLE_TESTS)
