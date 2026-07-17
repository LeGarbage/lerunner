cmake_minimum_required(VERSION 4.3.0)
project(lerunner VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(Gtkmm REQUIRED IMPORTED_TARGET gtkmm-4.0)
pkg_check_modules(GtkLayerShell REQUIRED IMPORTED_TARGET gtk4-layer-shell-0)
find_package(rapidfuzz REQUIRED)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/GCR_CMake/macros)
include(GlibCompileResourcesSupport)
set(RESOURCE_LIST
  style.css
)
compile_gresources(RESOURCE_FILE
                   XML_OUT
                   TYPE EMBED_C
                   RESOURCES ${RESOURCE_LIST}
                   SOURCE_DIR ${CMAKE_SOURCE_DIR}/assets
)

add_executable(${PROJECT_NAME}
  src/main.cpp
  src/window.cpp
  src/plugins/desktop-entries.cpp
  ${RESOURCE_FILE}
)

target_link_libraries(${PROJECT_NAME} PUBLIC PkgConfig::Gtkmm PRIVATE PkgConfig::GtkLayerShell PRIVATE rapidfuzz::rapidfuzz)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
