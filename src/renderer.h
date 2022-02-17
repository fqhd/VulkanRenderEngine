#ifndef RENDERER_H
#define RENDERER_H

#include "vulkan_helper.h"

typedef struct {
	GLFWwindow* window;
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
	int graphics_queue_index;
	int num_image_views;
} Renderer;

void init_vulkan(Renderer* renderer);

#endif