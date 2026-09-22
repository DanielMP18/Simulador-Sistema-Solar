cmake_minimum_required(VERSION 3.20)

project(SolarSystem LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)
find_program(GLAD_EXECUTABLE glad REQUIRED)

set(GLAD_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")

execute_process(
    COMMAND "${GLAD_EXECUTABLE}" --api "gl:core=3.3" --out-path "${GLAD_GENERATED_DIR}" c
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMAND_ERROR_IS_FATAL ANY
)

add_executable(SolarSystem
    src/main.cpp
    src/Planeta.cpp
    src/Janela.cpp
    src/Camera.cpp
    src/Fisica.cpp
    "${GLAD_GENERATED_DIR}/src/gl.c"
)

target_include_directories(SolarSystem PRIVATE
    include
    "${GLAD_GENERATED_DIR}/include"
)

target_link_libraries(SolarSystem
    OpenGL::GL
    glfw
    glm::glm
    ${CMAKE_DL_LIBS}
)