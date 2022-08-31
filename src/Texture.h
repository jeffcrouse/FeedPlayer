#pragma once

namespace feed {
	class Texture
	{
	public:
		Texture();
		~Texture();

		Texture& operator=(const Texture&) = delete;
		Texture(const Texture&) { init(); }

		void bind();
		void unbind();
		void set(unsigned char* data, unsigned int width, unsigned int height);

	protected:
		void init();
		unsigned int id;
	};
}