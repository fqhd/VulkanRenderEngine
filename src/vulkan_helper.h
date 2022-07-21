#ifndef VULKAN_HELPER_H
#define VULKAN_HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "utils.h"
#define GLM_FORCE_RADIANS
#include <cglm/cglm.h>
#include <cglm/mat4.h>

typedef struct {
    float x, y, z, u, v;
} Vertex;

typedef struct {
    uint32_t* indices;
    uint32_t numIndices;
    uint32_t numVertices;
    Vertex* vertices;
} MeshData;

typedef struct {
    VkBuffer vertexBuffer;
    VkBuffer indexBuffer;
    VkDeviceMemory vertexMemory;
    VkDeviceMemory indexMemory;
    uint32_t numIndices;
} GPUMesh;

typedef struct {
    VkCommandBuffer* commandBuffers;
} CommandBuffer;

typedef struct {
	float r;
    float g;
    float b;
} UBO;

typedef struct {
	VkRenderPass render_pass;
	VkPipeline graphics_pipeline;
	VkPipelineLayout pipeline_layout;
	VkDescriptorSetLayout descriptor_layout;
    VkDescriptorSet* descriptor_sets;
	VkBuffer* uniform_buffers;
	VkDeviceMemory* uniform_buffers_memory;
	UBO ubo;
} GraphicsPipeline;

typedef struct {
	GLFWwindow* window;
	VkInstance instance;
	VkPhysicalDevice physical_device;
	VkDevice logical_device;
	VkSurfaceKHR surface;
	VkSwapchainKHR swapchain;
	VkImageView* image_views;
	VkSurfaceCapabilitiesKHR capabilities;
	VkFramebuffer* framebuffers;
	VkCommandPool command_pool;
	VkCommandBuffer* command_buffers;
	int graphics_queue_index;
	int num_image_views;
	VkQueue graphics_queue;
	VkSemaphore* image_available_semaphores;
	VkSemaphore* render_finished_semaphores;
	VkFence* in_flight_fences;
    VkDescriptorPool descriptorPool;
	int current_frame;
    GraphicsPipeline pipeline;
} Vulkan;


void destroy_vulkan(Vulkan* v, GPUMesh* mesh, uint32_t meshCount);
void draw_frame(Vulkan* v, GPUMesh* meshes, uint32_t meshCount);
GPUMesh create_mesh(Vulkan* vulkan, MeshData* mesh);
void init_vulkan(Vulkan* v);

#endif