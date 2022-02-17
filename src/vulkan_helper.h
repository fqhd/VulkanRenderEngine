#ifndef VULKAN_HELPER_H
#define VULKAN_HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "utils.h"

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
	VkPipelineLayout pipeline_layout;
	int graphics_queue_index;
	int num_image_views;
	VkSemaphore* image_availalbe_semaphores;
	VkSemaphore* render_finished_semaphores;
	VkFence* fences_in_flight;
	VkFence* images_in_flight;
	int current_frame;
	VkQueue graphics_queue;
} Vulkan;

void pick_physical_device(Vulkan* v);
void get_graphics_queue(Vulkan* v);
void create_logical_device(Vulkan* v);
void create_instance(Vulkan* v, int validation_layers);
void create_swapchain(Vulkan* v);
void create_image_views(Vulkan* v);
void create_render_pass(Vulkan* v);
void create_graphics_pipeline(Vulkan* v);
void create_framebuffers(Vulkan* v);
void create_command_pool(Vulkan* v);
void create_command_buffers(Vulkan* v);
void create_sync_objects(Vulkan* v);
void destroy_vulkan(Vulkan* v);
void draw_frame(Vulkan* v);

#endif