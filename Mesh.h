#pragma once

namespace feed {
	class Mesh
	{
	public:
		~Mesh();
		void make_quad(float x, float y, float w, float h);
		void draw();

	protected:

		unsigned int VBO, VAO, EBO;
	};
}

