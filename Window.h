#pragma once
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace feed{
	class Window
	{
	public:
		void setup();



	protected:
		GLFWwindow* window;

		static void init();
		static bool glfwInited;
	};
}

