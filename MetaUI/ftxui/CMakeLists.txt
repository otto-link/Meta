cmake_minimum_required(VERSION 3.22)

project(meta_tui LANGUAGES CXX)

# -----------------------------------------------------------------------------
# Source files
# -----------------------------------------------------------------------------

add_library(meta_tui)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

target_sources(meta_tui
  PRIVATE
    ${SOURCES}
)

# -----------------------------------------------------------------------------
# Include directories
# -----------------------------------------------------------------------------

target_include_directories(meta_tui
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------

# Meta core dependency
target_link_libraries(meta_tui
  PUBLIC meta
)

# -----------------------------------------------------------------------------
# FTXUI dependency
# -----------------------------------------------------------------------------

include(FetchContent)

FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
  GIT_TAG v6.1.9
)

FetchContent_MakeAvailable(ftxui)

# Link required FTXUI components
target_link_libraries(meta_tui
  PUBLIC
    meta_common
    ftxui::screen
    ftxui::dom
    ftxui::component
)
