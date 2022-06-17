# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project(glfwTest)

# Include sub-projects.
#add_subdirectory ("GLFW")
include_directories(GLFW/include)
#include_directories(glew-2.1.0/include/GL)

#add_subdirectory(glfwTest "GLFW/include")




