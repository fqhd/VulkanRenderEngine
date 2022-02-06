#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void pick_physical_device(const VkInstance* instance, VkPhysicalDevice* physical_device){
	unsigned int physical_device_count;
	vkEnumeratePhysicalDevices(*instance, &physical_device_count, NULL);
	VkPhysicalDevice* physical_devices = malloc(sizeof(VkPhysicalDeviceProperties) * physical_device_count);
	vkEnumeratePhysicalDevices(*instance, &physical_device_count, physical_devices);

	if(physical_device_count == 0){
		printf("Error: Failed to get physical device\n");
	}

	*physical_device = *physical_devices;
}

int get_graphics_queue(const VkPhysicalDevice* physical_device){
	unsigned int queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(*physical_device, &queue_family_count, NULL);
	VkQueueFamilyProperties* queueFamilies = malloc(sizeof(VkQueueFamilyProperties) * queue_family_count);
	if(queueFamilies == NULL){
		printf("Failed to malloc space for queueFamilies\n");
	}
	vkGetPhysicalDeviceQueueFamilyProperties(*physical_device, &queue_family_count, queueFamilies);
	if(queue_family_count == 0){
		printf("Found no queue families in physical device\n");
	}

	for(unsigned int i = 0; i < queue_family_count; i++){
		if(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT){
			return i;
		}
	}
	return -1;
}

void create_logical_device(const VkPhysicalDevice* physical_device, VkDevice* logical_device, int graphics_queue_index){
	// Information about the creation of the graphics queue
	float priority = 1.0f;
	VkDeviceQueueCreateInfo queue_create_info;
	queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_create_info.queueFamilyIndex = graphics_queue_index;
	queue_create_info.queueCount = 1;
	queue_create_info.pQueuePriorities = &priority;
	queue_create_info.pNext = NULL;
	queue_create_info.flags = 0;

	VkDeviceCreateInfo device_create_info;
	device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_create_info.pQueueCreateInfos = &queue_create_info;
	device_create_info.queueCreateInfoCount = 1;
	device_create_info.pEnabledFeatures = NULL;
	device_create_info.enabledLayerCount = 0;
	device_create_info.ppEnabledLayerNames = NULL;
	device_create_info.enabledExtensionCount = 2;
	const char* extension_names[2];
	extension_names[0] = "VK_KHR_portability_subset";
	extension_names[1] = "VK_KHR_swapchain";
	device_create_info.ppEnabledExtensionNames = extension_names;
	device_create_info.pNext = NULL;
	device_create_info.flags = 0;

	if(vkCreateDevice(*physical_device, &device_create_info, NULL, logical_device) != VK_SUCCESS){
		printf("Failed to create device\n");
	}
}

void create_instance(VkInstance* instance, int validation_layers){
	VkInstanceCreateInfo create_info;
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = NULL;

	// Copying GLFW extensions as well as our custom extensions into a pp extension names array
	unsigned int glfw_extension_count = 0;
	const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
	if(glfw_extensions == NULL){
		printf("Failed to get glfw extensions\n");
	}

	unsigned int total_extension_count = glfw_extension_count + 1;
	const char** total_extensions = malloc(sizeof(const char*) * total_extension_count);
	memcpy(total_extensions, glfw_extensions, sizeof(const char*) * glfw_extension_count);
	total_extensions[total_extension_count - 1] = "VK_KHR_get_physical_device_properties2";
	create_info.enabledExtensionCount = total_extension_count;
	create_info.ppEnabledExtensionNames = total_extensions;
	create_info.enabledLayerCount = 0;
	create_info.flags = 0;
	create_info.pNext = NULL;

	const char* validation_layer_name = "VK_LAYER_KHRONOS_validation";

	if(validation_layers){
		create_info.enabledLayerCount = 1;
		create_info.ppEnabledLayerNames = &validation_layer_name;
	}

	if(vkCreateInstance(&create_info, NULL, instance) != VK_SUCCESS){
		printf("Failed to create instance\n");
	}

	free(total_extensions);
}

int main(int argc, char** argvs){
	GLFWwindow* window = NULL;
	VkInstance instance;
	VkPhysicalDevice physical_device;
	VkDevice logical_device;
	int graphics_queue_index = 0;

	if(glfwInit() != GLFW_TRUE){
		printf("Failed to initialize GLFW");
	};
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(800, 600, "Our Vulkan Window", NULL, NULL);

	// Initializing vulkan
	create_instance(&instance, 1);
	pick_physical_device(&instance, &physical_device);
	graphics_queue_index = get_graphics_queue(&physical_device);
	create_logical_device(&physical_device, &logical_device, graphics_queue_index);

	while(!glfwWindowShouldClose(window)){
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
