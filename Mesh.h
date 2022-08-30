#pragma once



namespace feed {
	class Mesh
	{
	public:
		~Mesh();
		void init();
		void draw();

	protected:

		unsigned int VBO, VAO, EBO;
	};
}

