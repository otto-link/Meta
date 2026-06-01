project(
  meta
  VERSION 0.0.1
  LANGUAGES CXX)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

target_compile_definitions(
  meta PUBLIC $<$<BOOL:${META_ENABLE_STD_TYPES}>:META_ENABLE_STD_TYPES>
              $<$<BOOL:${META_ENABLE_GLM_TYPES}>:META_ENABLE_GLM_TYPES>)

target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)

if(META_ENABLE_GLM_TYPES)
  target_link_libraries(${PROJECT_NAME} glm::glm)
endif()
