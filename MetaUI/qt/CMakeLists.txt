cmake_minimum_required(VERSION 3.22)

project(meta_qt LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

# -----------------------------------------------------------------------------
# Source files
# -----------------------------------------------------------------------------

add_library(meta_qt)

# --- sources GUI headers need to be added in add_executable, otherwise the moc
# won't parse them
file(GLOB_RECURSE INCLUDES
     ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

target_sources(meta_qt
  PRIVATE
    ${SOURCES} ${INCLUDES}
)

# -----------------------------------------------------------------------------
# Include directories
# -----------------------------------------------------------------------------

target_include_directories(meta_qt
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------

# Meta core dependency
target_link_libraries(meta_qt
  PUBLIC meta
)

# -----------------------------------------------------------------------------
# Qt dependency
# -----------------------------------------------------------------------------

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Link required FTXUI components
target_link_libraries(meta_qt
  PUBLIC
    meta_common
    Qt6::Core
    Qt6::Widgets
)