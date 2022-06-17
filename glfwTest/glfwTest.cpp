// glfwTest.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <math.h>

// OpenGL libraries and extensitons.
#include "glfwTest.h"
#define GLEW_STATIC // Need to define to be able to statically link.
#include <glew.h>
#include <glfw3.h>


#include <chrono>
#include <thread>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shader.h"
#include "vertex2d.h"
#include "board.h"

void error_callback(int error, const char* description)
{
	std::cerr << "Error: " << description << std::endl;
}

/*
	Prints the version of openGL on the system using the glfw window in the
	format X.X.X
*/
void printOpenGLVersion(GLFWwindow* window)
{
	int major, minor, rev;
	major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
	minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
	rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);

	std::cout << "OpenGL version     " << major << "." << minor << "." << rev << std::endl;
}

/*
	Creates the inital window and returns it in pWindow, makes the opengl context current and loads
	all the pointers to opengl functions.

	Returns 0 if this was successful. Otherwise returns -1 to indicate error.
*/
int setupGLFW(GLFWwindow** pWindow, int xSize, int ySize)
{
	GLFWwindow* window;
	
	// Enforce that opengl version must at least be 3.2
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		return -1;

	window = glfwCreateWindow(xSize, ySize, "My Window", NULL, NULL);

	if (!window) {
		glfwTerminate();
		std::cerr << "Error: Creating glfw window.\n";
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK)
		return -1;

	glfwSwapInterval(1);


	int nrAttributes;
	printOpenGLVersion(window);

	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;


	*pWindow = window;

	return 0;
}

void cleanupGLFW(GLFWwindow* window)
{
	// Unbinds the buffer.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glfwDestroyWindow(window);
	//glDeleteBuffers(,bufferHandle)

	// Terminate the GLFW library.
	glfwTerminate();
}


void calcOffset(Vector2D* current, Vector2D* result, float radians)
{
	result->x = (current->x * cosf(radians)) - (current->y * sinf(radians));
	result->y = (current->x * sinf(radians)) + (current->y * cosf(radians));
}


unsigned int generateTexture(const char* fileName)
{
	///	TEXTURES 
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	
	// load and generate the texture
	int x, y, nrChannels;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* data = stbi_load(fileName, &x, &y, &nrChannels, 4);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, (const void*)data);
		glGenerateMipmap(GL_TEXTURE_2D);
	
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	return texture;
}


int main()
{
	GLFWwindow* window;
	unsigned int width, height;
	width = 1200;
	height = 960;

	if (setupGLFW(&window, width, height))
		exit(EXIT_FAILURE);


	// Create and initialize the buffer
	Board board(width, height);
	std::cout << "Num vertices: " << board.numVertices() << std::endl;

	/* ---------- Generate the handles to the opengl objects ------------ */
	GLuint bufferHandle, vao, ebo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &bufferHandle);
	glGenBuffers(1, &ebo);

	// Set up the vertex array and bind the buffers and data to it.
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, bufferHandle);
	
	// Set up the vertex buffer and index buffer to store vertex data.
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * board.numVertices(), board.getVertexPointer(), GL_STREAM_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * board.numIndices(), board.getIndexPointer(), GL_STREAM_DRAW);

	// Enable the position data for the screen position and the texture position.
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
	glEnableVertexAttribArray(1);

	// Generate the texture for the current context.
	unsigned int texture = generateTexture("resources/tetris_block.png");

	/* ----------------------------------------------------- */
	ShaderProgram shaderProg;
	
	shaderProg.addShader("shaders/vertex.glsl", SHADER_TYPE::VERTEX_SHADER);
	shaderProg.addShader("shaders/frag.glsl", SHADER_TYPE::FRAGMENT_SHADER);
	shaderProg.Link();
	shaderProg.use();

	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(shaderProg.program(), "blockTexture"), 0);

	// Wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);

		// Bind and activate the block textures.
		glActiveTexture(GL_TEXTURE0); 
		glBindTexture(GL_TEXTURE_2D, texture);
		shaderProg.use();

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, board.numVertices(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);		
		

		///* Swap front and back buffers *
		glfwSwapBuffers(window);
		
		//* Poll for and process events *
		glfwPollEvents();
	}

	cleanupGLFW(window);
	return 0;
}
