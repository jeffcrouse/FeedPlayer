#pragma once
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>

namespace feed {
	class Shader
	{
	public:
		void init();
		void bind(double t);
		void unbind();

	protected:
		GLuint id;
	};
}

