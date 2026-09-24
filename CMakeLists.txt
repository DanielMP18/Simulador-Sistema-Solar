cmake_minimum_required(VERSION 3.20)

project(SolarSystem LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
include(FetchContent)

# GLFW (compilado localmente)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# GLM (matemática 3D header-only, baixado localmente)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
    GIT_SHALLOW TRUE
)
set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glm)

find_program(GLAD_EXECUTABLE glad HINTS "${CMAKE_CURRENT_SOURCE_DIR}/.venv/Scripts" REQUIRED)


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

if(MINGW)
    target_link_options(SolarSystem PRIVATE -static)
endif()