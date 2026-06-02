cmake_minimum_required(VERSION 3.22)

project(meta-root)

# -----------------------------------------------------------------------------
# Core options
# -----------------------------------------------------------------------------

option(META_ENABLE_TESTS "" ON)
option(META_ENABLE_STD_TYPES "Enable built-in STL type registrations" ON)
option(META_ENABLE_GLM_TYPES "Enable built-in GLM type registrations" ON)

# -----------------------------------------------------------------------------
# UI backends
# -----------------------------------------------------------------------------

option(META_ENABLE_FTXUI_UI "Enable FTXUI UI backend" OFF)
option(META_ENABLE_QT_UI "Enable QT UI backend" OFF)

# -----------------------------------------------------------------------------
# Status output
# -----------------------------------------------------------------------------

message(STATUS "┌─ Meta configuration ────────────────")
message(STATUS "│ STL types:     ${META_ENABLE_STD_TYPES}")
message(STATUS "│ GLM types:     ${META_ENABLE_GLM_TYPES}")
message(STATUS "│ FTXUI UI:      ${META_ENABLE_FTXUI_UI}")
message(STATUS "│ QT UI:         ${META_ENABLE_QT_UI}")
message(STATUS "│ Tests:         ${META_ENABLE_TESTS}")
message(STATUS "└─────────────────────────────────────")

# -----------------------------------------------------------------------------
# C++ standard
# -----------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------------
# Compiler flags
# -----------------------------------------------------------------------------

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

# -----------------------------------------------------------------------------
# Output directory
# -----------------------------------------------------------------------------

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------

find_package(nlohmann_json REQUIRED)

if(META_ENABLE_GLM_TYPES)
  find_package(glm REQUIRED)
endif()

# -----------------------------------------------------------------------------
# Core library
# -----------------------------------------------------------------------------

add_subdirectory(Meta)

# -----------------------------------------------------------------------------
# Optional UI backends
# -----------------------------------------------------------------------------

if(META_ENABLE_FTXUI_UI OR META_ENABLE_QT_UI)
  add_subdirectory(MetaUI/common)
endif()

if(META_ENABLE_FTXUI_UI)
  add_subdirectory(MetaUI/ftxui)
endif()

if(META_ENABLE_QT_UI)
  add_subdirectory(MetaUI/qt)
endif()

# -----------------------------------------------------------------------------
# Tests
# -----------------------------------------------------------------------------

if(META_ENABLE_TESTS)
  add_subdirectory(tests)
endif()
