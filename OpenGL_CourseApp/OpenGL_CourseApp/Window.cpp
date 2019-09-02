#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}

Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}

int Window::Initialise()
{
	//Initialise GLFW:
	if (!glfwInit())
	{
		cout << "GLFW Initialisation Failed!" << endl;
		glfwTerminate();
		return 1;
	}

	// Setup GLFW Windows Properties
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Core Profile= No Backwards compatibility:
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Allow forward compatiblity
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create the window
	mainWindow = glfwCreateWindow(width, height, "Test Window", NULL, NULL);
	if (!mainWindow)
	{
		printf("Error creating GLFW window!");
		glfwTerminate();
		return 1;
	}

	// Get buffer size information
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set the current context Set context for GLEW to use:
	glfwMakeContextCurrent(mainWindow);

	//Handle key + mouse Input:
	createCallbacks();
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Allow modern extension access 	//Allow modern extension features:
	glewExperimental = GL_TRUE;

	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		printf("Error: %s", glewGetErrorString(error));
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST);   //Activate Depth Buffer.

	// Create Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);

	glfwSetWindowUserPointer(mainWindow, this);
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, handleKeys);
	glfwSetCursorPosCallback(mainWindow, handleMouse);
}

GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}

void Window::handleKeys(GLFWwindow * window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			//cout << "Released\t:" << key << endl;

		}
		//cout << "Pressed\t:" << key << endl;
	}
	
}

void Window::handleMouse(GLFWwindow * window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mousedFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mousedFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;

	//cout << "X:\t" << theWindow->xChange << "Y:\t" << theWindow->yChange << endl;
}




Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}

