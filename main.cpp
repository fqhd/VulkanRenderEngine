#include "Window.hpp"

int main(){

	Window window(800, 600, "Window");

	while(!window.shouldClose()){
		window.processInput();
	}

	window.close();

	return 0;
}