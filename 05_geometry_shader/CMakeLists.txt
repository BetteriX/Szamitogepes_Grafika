cmake_minimum_required(VERSION 3.10)

project(05_Sample)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenGL REQUIRED)

# Include directories
include_directories(${OPENGL_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/../OpenGLTemplate/include)

# Add executable
add_executable(05_Sample Source.cpp)

# Link libraries
target_link_libraries(05_Sample ${OPENGL_LIBRARIES} ${CMAKE_SOURCE_DIR}/../OpenGLTemplate/lib/libGLEW.2.2.0.dylib ${CMAKE_SOURCE_DIR}/../OpenGLTemplate/lib/libglfw.3.4.dylib ${CMAKE_SOURCE_DIR}/../OpenGLTemplate/lib/libGLU.1.dylib)

# Copy shader files to build directory
configure_file(${CMAKE_SOURCE_DIR}/BuildHouseGeom.glsl ${CMAKE_BINARY_DIR}/BuildHouseGeom.glsl COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/GeometryFrag.glsl ${CMAKE_BINARY_DIR}/GeometryFrag.glsl COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/GeometryVert.glsl ${CMAKE_BINARY_DIR}/GeometryVert.glsl COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/SubdivisionGeom.glsl ${CMAKE_BINARY_DIR}/SubdivisionGeom.glsl COPYONLY) 