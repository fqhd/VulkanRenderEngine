#ifndef VULKAN_HELPER_H
#define VULKAN_HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "utils.h"
#include <cglm/cglm.h>

typedef struct {
	mat4 model;
	mat4 view;
	mat4 projection;
} MVP;

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
	VkQueue graphics_queue;
	VkSemaphore* image_available_semaphores;
	VkSemaphore* render_finished_semaphores;
	VkFence* in_flight_fences;
	int current_frame;
	VkBuffer vertex_buffer;
	VkDeviceMemory vertex_buffer_memory;
	VkBuffer index_buffer;
	VkDeviceMemory index_buffer_memory;
	VkDescriptorSetLayout descriptor_layout;
	VkBuffer* uniform_buffers;
	VkDeviceMemory* uniform_buffers_memory;
} Vulkan;

void create_window(Vulkan* v);
void pick_physical_device(Vulkan* v);
void get_graphics_queue_family_index(Vulkan* v);
void create_logical_device(Vulkan* v);
void create_instance(Vulkan* v, uint8_t validation_layers);
void create_swapchain(Vulkan* v);
void create_image_views(Vulkan* v);
void create_render_pass(Vulkan* v);
void create_graphics_pipeline(Vulkan* v);
void create_framebuffers(Vulkan* v);
void create_command_pool(Vulkan* v);
void create_command_buffers(Vulkan* v);
void create_sync_objects(Vulkan* v);
void destroy_vulkan(Vulkan* v);
void recreate_swapchain(Vulkan* v);
void draw_frame(Vulkan* v);
void cleanup_swapchain(Vulkan* v);
void create_vertex_buffer(Vulkan* v);
void create_index_buffer(Vulkan* v);
void init_vulkan(Vulkan* v);
void create_descriptor_layout(Vulkan* v);
void create_uniform_buffers(Vulkan* v);

#endif