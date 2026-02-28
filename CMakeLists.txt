cmake_minimum_required(VERSION 3.16)
project(TetrisSFML)

set(CMAKE_CXX_STANDARD 20)
# Absolute path to SFMLConfig.cmake
set(SFML_DIR "C:/SFML-2.6.2/lib/cmake/SFML")
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(TetrisSFML main.cpp)
target_link_libraries(TetrisSFML sfml-graphics sfml-window sfml-system)