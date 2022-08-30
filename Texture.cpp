#include "Texture.h"
#define GLEW_STATIC
#include <GL/glew.h>

using namespace feed;


// --------------------------------------------------------------------------------------
void Texture::init()
{
	glGenTextures(1, &id);

	glBindTexture(GL_TEXTURE_2D, id); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
}

// --------------------------------------------------------------------------------------
void Texture::set(unsigned char* data, unsigned int width, unsigned int height)
{
	glBindTexture(GL_TEXTURE_2D, id);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}


// --------------------------------------------------------------------------------------
void Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, id);
}


// --------------------------------------------------------------------------------------
void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}