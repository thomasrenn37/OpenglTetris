# CMakeList.txt : CMake project for glfwTest, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(SRC_SHADER_DIR "shaders")

# include and find the libary for GLFW
include_directories(libs/GLFW/include)
include_directories(libs/glew-2.1.0/include/GL)
include_directories(libs/stb/)

add_subdirectory(libs)
find_library(GLFW_LIB glfw3 libs/GLFW/lib)
find_library(GLEW_LIB glew32s libs/glew-2.1.0/lib/Release/x64)
find_package(OpenGl REQUIRED)


# Add source to this project's executable.
add_executable (glfwTest glfwTest.cpp glfwTest.h shader.h shader.cpp libs/stb/stb_image.h   "board.h" "board.cpp" "Texture.cpp" "Texture.h" "Text.h" "Text.cpp")

add_library(SHADER STATIC shader.h shader.cpp "Texture.cpp" "Texture.h" "Text.h" "Text.cpp")

target_link_libraries(glfwTest PRIVATE ${GLFW_LIB} ${GLEW_LIB} ${SHADER} opengl32)