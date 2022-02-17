#include "renderer.h"

void init_renderer(Renderer* renderer){
	if(glfwInit() != GLFW_TRUE){
		printf("Failed to initialize GLFW");
	};
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	renderer->vulkan.window = glfwCreateWindow(800, 600, "Our Vulkan Window", NULL, NULL);

	// Initializing vulkan
	create_instance(&renderer->vulkan, 1);
	pick_physical_device(&renderer->vulkan);
	get_graphics_queue(&renderer->vulkan);
	create_logical_device(&renderer->vulkan);
	glfwCreateWindowSurface(renderer->vulkan.instance, renderer->vulkan.window, NULL, &renderer->vulkan.surface);
	create_swapchain(&renderer->vulkan);
	create_image_views(&renderer->vulkan);
	create_render_pass(&renderer->vulkan);
	create_graphics_pipeline(&renderer->vulkan);
	create_framebuffers(&renderer->vulkan);
	create_command_pool(&renderer->vulkan);
	create_command_buffers(&renderer->vulkan);
	create_sync_objects(&renderer->vulkan);
}

void destroy_renderer(Renderer* renderer){
	destroy_vulkan(&renderer->vulkan);
}
