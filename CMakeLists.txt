cmake_minimum_required(VERSION 3.15)
project(clngine C CXX)

# Require C++17 for modern language features
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_SOURCES
    #${CMAKE_CURRENT_SOURCE_DIR}/src/
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/input.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gpumesh.cpp
)

# 2. Add header search paths (your code directory + GLAD headers)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glad/include
)

# 3. Locate system OpenGL framework
find_package(OpenGL REQUIRED)

include(FetchContent)

# 4. Handle GLFW (Auto-builds on Windows, links system package on Linux)
if(WIN32)
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.4
    )
    # Turn off compiling extra GLFW tools we don't need
    set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
    set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
    set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")
    FetchContent_MakeAvailable(glfw)
    set(GLFW_LIBRARY glfw)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLFW REQUIRED glfw3)
    set(GLFW_LIBRARY ${GLFW_LIBRARIES})
endif()

# 5. Handle GLM (Auto-downloads on Windows, links system package on Linux)
if(WIN32)
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        1.0.1
    )
    FetchContent_MakeAvailable(glm)
else()
    find_package(glm CONFIG REQUIRED)
endif()

# 6. Define the final application executable target
add_executable(${PROJECT_NAME} 
    ${PROJECT_SOURCES} 
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glad/glad.c
 )

# 7. Link third-party libraries and drivers to your application
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
    ${GLFW_LIBRARY}
    glm::glm
    OpenGL::GL
)

# 8. Link essential Linux system windowing and threading backend drivers
if(NOT WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE X11 pthread dl)
endif()