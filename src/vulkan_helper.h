#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void pick_physical_device(const VkInstance* instance, VkPhysicalDevice* physical_device);
void get_graphics_queue(const VkPhysicalDevice* physical_device, int* graphics_queue_index);
void create_logical_device(const VkPhysicalDevice* physical_device, VkDevice* logical_device, int graphics_queue_index);
void create_instance(VkInstance* instance, int validation_layers);
void create_swapchain(const VkPhysicalDevice* physical_device, const VkDevice* logical_device, VkSurfaceKHR* surface, VkSwapchainKHR* swapchain);
void create_image_views(const VkDevice* logical_device, const VkSwapchainKHR* swapchain, VkImageView** image_views);
