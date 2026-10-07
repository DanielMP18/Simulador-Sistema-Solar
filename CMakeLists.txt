cmake_minimum_required(VERSION 3.20)

project(SistemaSolar VERSION 1.0.0 LANGUAGES C CXX)

include(CTest)
include(FetchContent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glfw)

set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glm)

find_program(
    GLAD_EXECUTABLE
    NAMES glad
    HINTS
        "${CMAKE_CURRENT_SOURCE_DIR}/.venv/bin"
        "${CMAKE_CURRENT_SOURCE_DIR}/.venv/Scripts"
    REQUIRED
)

set(GLAD_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
execute_process(
    COMMAND "${GLAD_EXECUTABLE}" --api "gl:core=3.3" --out-path "${GLAD_GENERATED_DIR}" c
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMAND_ERROR_IS_FATAL ANY
)

add_library(solar_core
    src/Camera.cpp
    src/Fisica.cpp
    src/SistemaSolar.cpp
)
target_include_directories(solar_core PUBLIC include)
target_link_libraries(solar_core PUBLIC glm::glm)

add_executable(SolarSystem
    src/Aplicacao.cpp
    src/Janela.cpp
    src/Renderizador.cpp
    src/main.cpp
    "${GLAD_GENERATED_DIR}/src/gl.c"
)
target_include_directories(SolarSystem PRIVATE "${GLAD_GENERATED_DIR}/include")
target_link_libraries(SolarSystem PRIVATE solar_core OpenGL::GL glfw ${CMAKE_DL_LIBS})

if(MSVC)
    target_compile_options(solar_core PRIVATE /W4 /permissive-)
    target_compile_options(SolarSystem PRIVATE /W4 /permissive-)
else()
    target_compile_options(solar_core PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(SolarSystem PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(BUILD_TESTING)
    add_executable(fisica_tests tests/test_fisica.cpp)
    target_link_libraries(fisica_tests PRIVATE solar_core)
    if(MSVC)
        target_compile_options(fisica_tests PRIVATE /W4 /permissive-)
    else()
        target_compile_options(fisica_tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    add_test(NAME fisica_tests COMMAND fisica_tests)
endif()

if(MINGW)
    target_link_options(SolarSystem PRIVATE -static)
endif()
