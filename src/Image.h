#pragma once
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace feed {
	class Image
	{
	public:
		void load(std::string path);
	};
}
