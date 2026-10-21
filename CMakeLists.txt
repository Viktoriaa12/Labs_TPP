
cmake_minimum_required(VERSION 3.25)

project(ProjectileMotion LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 3 COMPONENTS Graphics Window System CONFIG REQUIRED)

add_executable(ProjectileMotion main.cpp)

target_link_libraries(ProjectileMotion PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

