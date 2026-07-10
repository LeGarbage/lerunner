cmake_minimum_required(VERSION 3.10.0)
project(lerunner VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)

add_executable(lerunner
  src/main.cpp
  src/window.cpp
  src/plugins/desktop-entries.cpp
)

find_package(PkgConfig REQUIRED)

pkg_check_modules(Gtkmm REQUIRED IMPORTED_TARGET gtkmm-4.0)
target_link_libraries(lerunner PkgConfig::Gtkmm)

pkg_check_modules(GtkLayerShell REQUIRED IMPORTED_TARGET gtk4-layer-shell-0)
target_link_libraries(lerunner PkgConfig::GtkLayerShell)

find_package(rapidfuzz REQUIRED)
target_link_libraries(lerunner rapidfuzz::rapidfuzz)

find_package(XdgUtils REQUIRED)
target_link_libraries(lerunner -L${XDGUTILS_LIBDIR} XdgUtilsDesktopEntry)

install(TARGETS lerunner DESTINATION bin)
