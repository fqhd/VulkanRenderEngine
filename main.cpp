#include "Window.hpp"

int main(){
	Window window(800, 600, "Our Vulkan Window");
	while(!window.shouldClose()){
		window.processInput();
	}
	window.destroy();
}