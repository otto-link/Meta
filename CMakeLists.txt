cmake_minimum_required(VERSION 3.22)

project(meta-root)

option(META_ENABLE_TESTS "" ON)
option(META_ENABLE_STD_TYPES "Enable built-in STL type registrations" ON)

if(META_ENABLE_STD_TYPES)
  message(STATUS "[Meta] STL type support: ENABLED")
else()
  message(STATUS "[Meta] STL type support: DISABLED")
endif()

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

find_package(nlohmann_json REQUIRED)

# --- library

add_subdirectory(Meta)

# --- everything else...

if(META_ENABLE_TESTS)
  add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
endif(META_ENABLE_TESTS)
