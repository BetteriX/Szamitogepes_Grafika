cmake_minimum_required(VERSION 3.10)

project(08_Sample)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenGL REQUIRED)

# Include directories
include_directories(${OPENGL_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/../OpenGLTemplate/include)

# Add executable
add_executable(08_Sample Source.cpp)

# Link libraries
target_link_libraries(08_Sample ${OPENGL_LIBRARIES} ${CMAKE_SOURCE_DIR}/../OpenGLTemplate/lib/libGLEW.2.2.0.dylib ${CMAKE_SOURCE_DIR}/../OpenGLTemplate/lib/libglfw.3.4.dylib ${CMAKE_SOURCE_DIR}/../OpenGLTemplate/lib/libGLU.1.dylib)

# Copy shader files to build directory
configure_file(${CMAKE_SOURCE_DIR}/CurveFragShader.glsl ${CMAKE_BINARY_DIR}/CurveFragShader.glsl COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/CurveTessContShader.glsl ${CMAKE_BINARY_DIR}/CurveTessContShader.glsl COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/CurveTessEvalShader.glsl ${CMAKE_BINARY_DIR}/CurveTessEvalShader.glsl COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/CurveVertShader.glsl ${CMAKE_BINARY_DIR}/CurveVertShader.glsl COPYONLY) 
configure_file(${CMAKE_SOURCE_DIR}/QuadScreenFragShader.glsl ${CMAKE_BINARY_DIR}/QuadScreenFragShader.glsl COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/QuadScreenVertShader.glsl ${CMAKE_BINARY_DIR}/QuadScreenVertShader.glsl COPYONLY) 