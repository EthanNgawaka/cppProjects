# Minimum required version of CMake
cmake_minimum_required(VERSION 3.24)

# Project name and version
project(main VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Package preferences
set(OpenGL_GL_PREFERENCE "GLVND")

# Find the required packages
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

# Include the directories of the found packages
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ../common)

# Add your source files here 
add_executable(main main.cpp ../common/shader.hpp)

# Link against the required libraries
target_link_libraries(main ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES} glm::glm)

