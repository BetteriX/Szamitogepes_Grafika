cmake_minimum_required(VERSION 3.10)

project(12_Sample)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenGL REQUIRED)

# Include directories
include_directories(${OPENGL_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/../OpenGLTemplate/include)

# Add executable
add_executable(12_Sample Source.cpp)

# Link libraries
target_link_libraries(12_Sample ${OPENGL_LIBRARIES} ${CMAKE_SOURCE_DIR}/../OpenGLTemplate/lib/libGLEW.2.2.0.dylib ${CMAKE_SOURCE_DIR}/../OpenGLTemplate/lib/libglfw.3.4.dylib ${CMAKE_SOURCE_DIR}/../OpenGLTemplate/lib/libGLU.1.dylib)

# Copy shader files to build directory
configure_file(${CMAKE_SOURCE_DIR}/ColorShiftComp.glsl ${CMAKE_BINARY_DIR}/ColorShiftComp.glsl COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/QuadScreenFrag.glsl ${CMAKE_BINARY_DIR}/QuadScreenFrag.glsl COPYONLY) 
configure_file(${CMAKE_SOURCE_DIR}/QuadScreenVert.glsl ${CMAKE_BINARY_DIR}/QuadScreenVert.glsl COPYONLY) 