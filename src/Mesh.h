#pragma once

namespace feed {
	class Mesh
	{
	public:

		Mesh() = default;
		~Mesh();

		Mesh& operator=(const Mesh&) = delete;
		//Mesh(const Mesh&) = delete;

		void make_quad(float x, float y, float w, float h);
		void draw();

	protected:

		unsigned int VBO, VAO, EBO;
	};
}

