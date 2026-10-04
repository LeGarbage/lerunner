add_subdirectory(matcher)

find_package(PkgConfig REQUIRED)
pkg_check_modules(Gtkmm REQUIRED IMPORTED_TARGET gtkmm-4.0)
pkg_check_modules(GtkLayerShell REQUIRED IMPORTED_TARGET gtk4-layer-shell-0)

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
  main.cpp
  window.cpp
  plugins/desktop-entries.cpp
  plugins/system-actions.cpp
  ${RESOURCE_FILE}
)

target_link_libraries(${PROJECT_NAME} lib${PROJECT_NAME}-matcher PkgConfig::Gtkmm PkgConfig::GtkLayerShell)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
