#include "vulkan_helper.h"

int main(int argc, char** argvs){
	GLFWwindow* window = NULL;
	VkInstance instance;
	VkPhysicalDevice physical_device;
	VkDevice logical_device;
	VkSurfaceKHR surface;
	VkSwapchainKHR swapchain;
	VkImageView* image_views;
	VkRenderPass render_pass;
	VkSurfaceCapabilitiesKHR capabilities;
	VkFramebuffer* framebuffers;
	VkCommandPool command_pool;
	VkPipeline graphics_pipeline;
	VkCommandBuffer* command_buffers;
	int graphics_queue_index = 0;
	int num_image_views;

	if(glfwInit() != GLFW_TRUE){
		printf("Failed to initialize GLFW");
	};
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(800, 600, "Our Vulkan Window", NULL, NULL);

	// Initializing vulkan
	create_instance(&instance, 1);
	pick_physical_device(&instance, &physical_device);
	get_graphics_queue(&physical_device, &graphics_queue_index);
	create_logical_device(&physical_device, &logical_device, graphics_queue_index);
	glfwCreateWindowSurface(instance, window, NULL, &surface);
	create_swapchain(&physical_device, &logical_device, &surface, &swapchain, &capabilities);
	create_image_views(&logical_device, &swapchain, &image_views, &num_image_views);
	create_render_pass(&logical_device, &render_pass);
	create_graphics_pipeline(&logical_device, &capabilities.currentExtent, &render_pass, &graphics_pipeline);
	create_framebuffers(&framebuffers, image_views, num_image_views, &render_pass, &capabilities, &logical_device);
	create_command_pool(&logical_device, graphics_queue_index, &command_pool);
	create_command_buffers(&logical_device, num_image_views, &command_pool, &graphics_pipeline, &capabilities, &render_pass, framebuffers, command_buffers);

	while(!glfwWindowShouldClose(window)){
		glfwPollEvents();
	}

	free(image_views);

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}