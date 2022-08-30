#pragma once

namespace feed {
	class Shader
	{
	public:
		void init();
		void bind(double t);
		void unbind();

	protected:
		unsigned int id;
	};
}

