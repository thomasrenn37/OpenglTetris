# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project (glfwTest)

# Include sub-projects.
add_subdirectory (glfwTest)

add_custom_target(
    
    copy-files ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/shaders ${PROJECT_BINARY_DIR}/${PROJECT_NAME}/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/resources ${PROJECT_BINARY_DIR}/${PROJECT_NAME}/resources
    COMMENT "Copying resource and shaders files"
)

add_dependencies(glfwTest copy-files)