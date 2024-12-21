cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(CMakeTest)

find_package( OpenGL REQUIRED )

# GLFW Config
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ONLY_LIBS  ON CACHE BOOL "" FORCE)

# GLEW Config
set(BUILD_UTILS OFF CACHE BOOL "" FORCE)

# GLM Config
set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)


# Libraries
add_subdirectory(extern/glew-2.1.0/build/cmake)
add_definitions(-DGLEW_STATIC)
include_directories(extern/glew-2.1.0/include)

add_subdirectory(extern/glfw)
add_subdirectory(extern/glm)
add_subdirectory(extern/spdlog)
add_subdirectory(extern/imgui)
add_subdirectory(extern/stb-cmake)

# Sources
# MY_SOURCES is defined to be a list of all the source files in src
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable("${CMAKE_PROJECT_NAME}")
set_property(TARGET "${CMAKE_PROJECT_NAME}" PROPERTY CXX_STANDARD 23)
target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC GLFW_INCLUDE_NONE=1) # Don't include GLFW's OpenGL headers

# Configure resource files macro
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC
        RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/res/"
        _DEBUG
    )
# TODO: Configure this properly
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC
        RESOURCES_PATH="./res/"
    )
    add_custom_command(
        TARGET "${CMAKE_PROJECT_NAME}" POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/res
                ${CMAKE_CURRENT_BINARY_DIR}/res)
endif()

# Final Link

target_sources("${CMAKE_PROJECT_NAME}" PRIVATE ${MY_SOURCES} )
target_include_directories("${CMAKE_PROJECT_NAME}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/glew-2.1.0/include")
target_include_directories("${CMAKE_PROJECT_NAME}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE glm glfw glew_s spdlog imgui stb)

