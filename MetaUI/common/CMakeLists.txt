cmake_minimum_required(VERSION 3.22)

project(meta_common LANGUAGES CXX)

# -----------------------------------------------------------------------------
# Source files
# -----------------------------------------------------------------------------

add_library(meta_common)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

target_sources(meta_common
  PRIVATE
    ${SOURCES}
)

# -----------------------------------------------------------------------------
# Include directories
# -----------------------------------------------------------------------------

target_include_directories(meta_common
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------

# Meta core dependency
target_link_libraries(meta_common
  PUBLIC meta
)
