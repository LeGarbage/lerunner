cmake_minimum_required(VERSION 3.10.0)
project(lerunner VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME}
  src/main.cpp
  src/window.cpp
  src/desktop-entries.cpp
)

find_package(PkgConfig REQUIRED)

pkg_check_modules(Gtkmm REQUIRED IMPORTED_TARGET gtkmm-4.0)
target_link_libraries(${PROJECT_NAME} PkgConfig::Gtkmm)

pkg_check_modules(GtkLayerShell REQUIRED IMPORTED_TARGET gtk4-layer-shell-0)
target_link_libraries(${PROJECT_NAME} PkgConfig::GtkLayerShell)

find_package(XdgUtils REQUIRED)
target_link_libraries(${PROJECT_NAME} XdgUtilsDesktopEntry)
