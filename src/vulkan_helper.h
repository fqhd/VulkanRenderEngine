#ifndef VULKAN_HELPER_H
#define VULKAN_HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "utils.h"

void pick_physical_device(const VkInstance* instance, VkPhysicalDevice* physical_device);
void get_graphics_queue(const VkPhysicalDevice* physical_device, int* graphics_queue_index);
void create_logical_device(const VkPhysicalDevice* physical_device, VkDevice* logical_device, int graphics_queue_index);
void create_instance(VkInstance* instance, int validation_layers);
void create_swapchain(const VkPhysicalDevice* physical_device, const VkDevice* logical_device, VkSurfaceKHR* surface, VkSwapchainKHR* swapchai, VkSurfaceCapabilitiesKHR* capabilities);
void create_image_views(const VkDevice* logical_device, const VkSwapchainKHR* swapchain, VkImageView** image_views, int* num_image_views);
void create_render_pass(const VkDevice* device, VkRenderPass* render_pass);
void create_graphics_pipeline(const VkDevice* device, const VkExtent2D* extent, const VkRenderPass* render_pass, VkPipeline* graphics_pipeline);
void create_framebuffers(VkFramebuffer** framebuffers, const VkImageView* image_views, int num_image_views, const VkRenderPass* render_pass, const VkSurfaceCapabilitiesKHR* capabilities, const VkDevice* device);
void create_command_pool(const VkDevice* device, int graphics_queue_index, VkCommandPool* pool);
void create_command_buffers(const VkDevice* device, int num_swapchain_mages, const VkCommandPool* command_pool, const VkPipeline* graphics_pipeline, const VkSurfaceCapabilitiesKHR* capabilities, const VkRenderPass* render_pass, const VkFramebuffer* framebuffers, VkCommandBuffer* command_buffers);

#endif