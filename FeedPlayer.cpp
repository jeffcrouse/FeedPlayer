#include "Window.h"

int main() {
	try {
		feed::Window window;
		window.run();
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}