#pragma once

namespace feed {
	class Shader
	{
	public:
		Shader& operator=(const Shader&) = delete;
		Shader(const Shader&) = delete;
		Shader() = default;

		void init();
		void bind(double t);
		void unbind();

	protected:
		unsigned int id;
	};
}

