cmake_minimum_required(VERSION 3.20)

project(SolarSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(SolarSystem
    src/main.cpp
    src/Planeta.cpp
    src/Janela.cpp
    src/Camera.cpp
    src/Fisica.cpp
)

# Ensina o compilador a buscar arquivos na pasta include/
target_include_directories(SolarSystem PRIVATE include)

target_link_libraries(SolarSystem
    OpenGL::GL
    glfw
)