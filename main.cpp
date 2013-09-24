#include <iostream>
#include "Renderer.h"
#include <GL/glew.h>
#include <GL/glfw.h>

#include <Eigen/Core>

const int Window_Width=1024;
const int Window_Height=768;

int main()
{
	if(!glfwInit() )
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4); // 4x antialiasing
	//	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3); // OpenGL 3.2
	//	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
	//	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL

	// Open a window and create its OpenGL context
	if(!glfwOpenWindow(Window_Width, Window_Height, 0,0,0,0, 32,0, GLFW_WINDOW))
	{
		std::cerr << "Failed to open GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Initialize GLEW
	glewExperimental=true; // Needed in core profile
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	glfwSetWindowTitle("ShipRace");

	// Ensure we can capture the escape key being pressed below
	glfwEnable( GLFW_STICKY_KEYS );

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	// Initialize Mouse Position
	//glfwDisable( GLFW_MOUSE_CURSOR);
	glfwSetMousePos(Window_Width/2, Window_Height/2);

	Renderer renderer(Window_Width, Window_Height);
	renderer.init();

	//VSync
	//glfwSwapInterval(1);
	do
	{
		renderer.render();

		// Swap buffers
		glfwSwapBuffers();
	}
	while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS && glfwGetWindowParam( GLFW_OPENED ) );


	glfwTerminate();
	return 0;
}

