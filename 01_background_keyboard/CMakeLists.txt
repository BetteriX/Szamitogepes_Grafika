cmake_minimum_required(VERSION 3.10)

project(01_Sample)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenGL REQUIRED)

# Include directories
include_directories(${OPENGL_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/../OpenGLTemplate/include)

# Add executable
add_executable(01_Sample Source.cpp)

# Link libraries
IF (WIN32)
target_link_libraries(01_Sample ${OPENGL_LIBRARIES} ${CMAKE_SOURCE_DIR}/../OpenGLTemplate/lib/glew32.lib ${CMAKE_SOURCE_DIR}/../OpenGLTemplate/lib/glfw3.lib)
ELSEIF(APPLE)
target_link_libraries(01_Sample ${OPENGL_LIBRARIES} ${CMAKE_SOURCE_DIR}/../OpenGLTemplate/lib/libGLEW.2.2.0.dylib ${CMAKE_SOURCE_DIR}/../OpenGLTemplate/lib/libglfw.3.4.dylib)
ELSE()
        # Linux // Assumed??
ENDIF()

# Copy shader files to build directory
#configure_file(${CMAKE_SOURCE_DIR}/vertex_shader.glsl ${CMAKE_BINARY_DIR}/vertex_shader.glsl COPYONLY)
#configure_file(${CMAKE_SOURCE_DIR}/fragment_shader.glsl ${CMAKE_BINARY_DIR}/fragment_shader.glsl COPYONLY) 