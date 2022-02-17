#include "renderer.h"

int main(int argc, char** argvs){
	Renderer renderer;

	init_renderer(&renderer);

	while(!glfwWindowShouldClose(renderer.vulkan.window)){
		glfwPollEvents();
	}

	destroy_renderer(&renderer);
	return 0;
}