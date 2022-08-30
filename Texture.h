#pragma once

namespace feed {
	class Texture
	{
	public:
		void init();
		void bind();
		void unbind();
		void set(unsigned char* data, unsigned int width, unsigned int height);
	protected:
		unsigned int id;

	};
}