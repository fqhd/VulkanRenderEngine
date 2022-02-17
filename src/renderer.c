#include "renderer.h"

void init_renderer(Renderer* renderer){
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
	create_graphics_pipeline(&renderer->logical_device, &renderer->capabilities.currentExtent, &renderer->render_pass, &renderer->graphics_pipeline, &renderer->pipeline_layout);
	create_framebuffers(&renderer->framebuffers, renderer->image_views, renderer->num_image_views, &renderer->render_pass, &renderer->capabilities, &renderer->logical_device);
	create_command_pool(&renderer->logical_device, renderer->graphics_queue_index, &renderer->command_pool);
	create_command_buffers(&renderer->logical_device, renderer->num_image_views, &renderer->command_pool, &renderer->graphics_pipeline, &renderer->capabilities, &renderer->render_pass, renderer->framebuffers, renderer->command_buffers);
}

void destroy_renderer(Renderer* renderer){
	vkDestroyCommandPool(renderer->logical_device, renderer->command_pool, NULL);

	for (int i = 0; i < renderer->num_image_views; i++) {
		vkDestroyFramebuffer(renderer->logical_device, renderer->framebuffers[i], NULL);
	}

	vkDestroyRenderPass(renderer->logical_device, renderer->render_pass, NULL);

	vkDestroyPipeline(renderer->logical_device, renderer->graphics_pipeline, NULL);

	vkDestroyPipelineLayout(renderer->logical_device, renderer->pipeline_layout, NULL);

	for (int i = 0; i < renderer->num_image_views; i++) {
		vkDestroyImageView(renderer->logical_device, renderer->image_views[i], NULL);
	}

	vkDestroySwapchainKHR(renderer->logical_device, renderer->swapchain, NULL);

	vkDestroyDevice(renderer->logical_device, NULL);

	vkDestroySurfaceKHR(renderer->instance, renderer->surface, NULL);

	vkDestroyInstance(renderer->instance, NULL);

	glfwDestroyWindow(renderer->window);
	glfwTerminate();
}
