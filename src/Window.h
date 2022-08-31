#pragma once
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp> 
#include <vector>

#include "Video.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"

namespace feed{
	class Window
	{
	public:
		Window();
		~Window();

		Window& operator=(const Window&) = delete;
		Window(const Window&) = delete;

		void setup();
		void draw();
		void run();
		void set_title(std::string title);
		void set_size(unsigned int width, unsigned int height);
		void set_fullscreen(bool fullscreen);
		inline bool is_fullscreen() { return glfwGetWindowMonitor(window) != nullptr; }
		inline void toggle_fullscreen() { set_fullscreen(!is_fullscreen()); }


		void on_key(int key, int scancode, int action, int mods);
		void on_mouse(double xpos, double ypos);

	protected:
		GLFWwindow* window = nullptr;
		GLFWmonitor* monitor = nullptr;


		double lastTime = 0;
		double deltaTime = 0;
		float framerate;
		glm::vec3 clearColor;
		int width;
		int height;
		int xpos;
		int ypos;

		std::vector<feed::Video> videos;
		std::vector<feed::Mesh> meshes;
		std::vector<feed::Texture> textures;
		feed::Shader shader;
	};
}

