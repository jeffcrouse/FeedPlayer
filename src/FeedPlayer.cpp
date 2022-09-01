#include <filesystem>
#include "Window.h"


int main(int argc, char* argv[]) 
{
	auto path = std::filesystem::path(argv[0])
		.parent_path()
		.parent_path()
		.parent_path();
	auto dir = std::filesystem::weakly_canonical(path);
	
	std::filesystem::current_path(dir);
	std::cout << "Current path is " << std::filesystem::current_path() << std::endl;

	try {
		feed::Window app;
		app.run();
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		return -1;
	}
	return 0;
}