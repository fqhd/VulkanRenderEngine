#include "renderer.h"

int main(int argc, char** argvs){
	Renderer renderer;

	init_vulkan(&renderer);

	while(!glfwWindowShouldClose(renderer.window)){
		glfwPollEvents();
	}

	glfwDestroyWindow(renderer.window);
	glfwTerminate();
	return 0;
}