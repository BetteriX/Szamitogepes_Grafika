cmake_minimum_required(VERSION 3.10)

project(11_Sample)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenGL REQUIRED)

# Include directories
include_directories(${OPENGL_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/../OpenGLTemplate/include)

# Add executable
add_executable(11_Sample Source.cpp)

# Link libraries
target_link_libraries(11_Sample ${OPENGL_LIBRARIES} ${CMAKE_SOURCE_DIR}/../OpenGLTemplate/lib/libGLEW.2.2.0.dylib ${CMAKE_SOURCE_DIR}/../OpenGLTemplate/lib/libglfw.3.4.dylib ${CMAKE_SOURCE_DIR}/../OpenGLTemplate/lib/libGLU.1.dylib ${CMAKE_SOURCE_DIR}/../OpenGLTemplate/lib/libsoil2-debug.dylib)

# Copy shader files to build directory
configure_file(${CMAKE_SOURCE_DIR}/vertexShader.glsl ${CMAKE_BINARY_DIR}/vertexShader.glsl COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/fragmentShader.glsl ${CMAKE_BINARY_DIR}/fragmentShader.glsl COPYONLY) 