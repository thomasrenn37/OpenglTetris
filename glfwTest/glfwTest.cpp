// glfwTest.cpp : Defines the entry point for the application.
//

#include <iostream>

// OpenGL libraries and extensitons.
#include "glfwTest.h"
#define GLEW_STATIC // Need to define to be able to statically link.
#include <glew.h>
#include <glfw3.h>

#include "board.h"

void error_callback(int error, const char* description)
{
	std::cerr << "Error: " << description << std::endl;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Board* board = static_cast<Board*>(glfwGetWindowUserPointer(window));

	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(window, true);
		else if (key == GLFW_KEY_W)
		{
			board->Flip();
		}
		else if (key == GLFW_KEY_A)
		{
			board->SetMoveDirection(-1, 0);
		}
		else if (key == GLFW_KEY_S)
		{
			board->SetMoveDirection(0, -1);
		}
		else if (key == GLFW_KEY_D)
		{
			board->SetMoveDirection(1, 0);
		}
		// Drop the piece
		else if (key == GLFW_KEY_SPACE)
		{
			board->Drop();
		}
	}
	else if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_W)
		{

		}
		else if (key == GLFW_KEY_A)
		{
			board->SetMoveDirection(0, 0);
		}
		else if (key == GLFW_KEY_S)
		{
			board->SetMoveDirection(0, 0);
		}
		else if (key == GLFW_KEY_D)
		{
			board->SetMoveDirection(0, 0);
		}
	}
	else if (action == GLFW_REPEAT)
	{
		if (key == GLFW_KEY_A)
		{
			board->SetMoveDirection(-1, 0);
		}
		else if (key == GLFW_KEY_S)
		{
			board->SetMoveDirection(0, -1);
		}
		else if (key == GLFW_KEY_D)
		{
			board->SetMoveDirection(1, 0);
		}
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
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

	if (!glfwInit())
		return -1;

	// Enforce that opengl version must at least be 3.2
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	

	glfwSetErrorCallback(error_callback);

	// Set up the window
	window = glfwCreateWindow(xSize, ySize, "My Window", NULL, NULL);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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

	// Set up key processing call_back
	glfwSetKeyCallback(window, key_callback);

	*pWindow = window;

	return 0;
}

void cleanupGLFW(GLFWwindow* window)
{	
	glfwDestroyWindow(window);

	// Terminate the GLFW library.
	glfwTerminate();
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
	glfwSetWindowUserPointer(window, &board);

	std::cout << "Num vertices: " << board.numVertices() << std::endl;

	// Wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Main game loop
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		
		// Poll for and process events 
		glfwPollEvents();

		// Update the board
		board.Update();

		// Render to the screen
		board.Render();

		// Swap front and back buffers 
		glfwSwapBuffers(window);
	}

	cleanupGLFW(window);
	return 0;
}
