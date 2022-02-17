#include "renderer.h"

void init_vulkan(Renderer* renderer){
	if(glfwInit() != GLFW_TRUE){
		printf("Failed to initialize GLFW");
	};
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	renderer->window = glfwCreateWindow(800, 600, "Our Vulkan Window", NULL, NULL);

	// Initializing vulkan
	create_instance(&renderer->instance, 1);
	pick_physical_device(&renderer->instance, &renderer->physical_device);
	get_graphics_queue(&renderer->physical_device, &renderer->graphics_queue_index);
	create_logical_device(&renderer->physical_device, &renderer->logical_device, renderer->graphics_queue_index);
	glfwCreateWindowSurface(renderer->instance, renderer->window, NULL, &renderer->surface);
	create_swapchain(&renderer->physical_device, &renderer->logical_device, &renderer->surface, &renderer->swapchain, &renderer->capabilities);
	create_image_views(&renderer->logical_device, &renderer->swapchain, &renderer->image_views, &renderer->num_image_views);
	create_render_pass(&renderer->logical_device, &renderer->render_pass);
	create_graphics_pipeline(&renderer->logical_device, &renderer->capabilities.currentExtent, &renderer->render_pass, &renderer->graphics_pipeline);
	create_framebuffers(&renderer->framebuffers, renderer->image_views, renderer->num_image_views, &renderer->render_pass, &renderer->capabilities, &renderer->logical_device);
	create_command_pool(&renderer->logical_device, renderer->graphics_queue_index, &renderer->command_pool);
	create_command_buffers(&renderer->logical_device, renderer->num_image_views, &renderer->command_pool, &renderer->graphics_pipeline, &renderer->capabilities, &renderer->render_pass, renderer->framebuffers, renderer->command_buffers);
	
}
