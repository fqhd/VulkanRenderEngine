#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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
	return 0;
}

int main(int argc, char** argvs){
	GLFWwindow* window = NULL;
	VkInstance instance;
	if(glfwInit() != GLFW_TRUE){
		printf("Failed to initialize GLFW");
	};
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(800, 600, "Our Vulkan Window", NULL, NULL);

	create_instance(&instance, 1);

	while(!glfwWindowShouldClose(window)){
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
