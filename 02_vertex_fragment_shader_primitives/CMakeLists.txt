cmake_minimum_required(VERSION 3.16)

project(02_Sample)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenGL REQUIRED)

# Include directories
include_directories(${OPENGL_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/../OpenGLTemplate/include)

# Add executable with all needed sources
add_executable(02_Sample .. Source.cpp)

# Link libraries manually — Linux .so versions
target_link_libraries(02_Sample
    ${OPENGL_LIBRARIES}
    /usr/lib/x86_64-linux-gnu/libGLEW.so
    /usr/lib/x86_64-linux-gnu/libglfw.so
    /usr/lib/x86_64-linux-gnu/libGLU.so
)

# Copy shader files to build directory
configure_file(${CMAKE_SOURCE_DIR}/vertexShader.glsl ${CMAKE_BINARY_DIR}/vertexShader.glsl COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/fragmentShader.glsl ${CMAKE_BINARY_DIR}/fragmentShader.glsl COPYONLY)