#include <stdio.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void createInstance(VkInstance* instance){
	validationLayersEnabled = enableValidationLayers;
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;
	appInfo.pNext = nullptr;
	
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	// Copying GLFW extensions as well as our custom extensions into a pp extension names array
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	const char* extension_name = "VK_KHR_get_physical_device_properties2";
	uint32_t numExtensions = glfwExtensionCount+1;
	char** global_extensions;
	global_extensions = (char**)malloc(sizeof(char*)*(glfwExtensionCount+1));
	memcpy(global_extensions, glfwExtensions, sizeof(char*)*glfwExtensionCount);
	*(global_extensions + glfwExtensionCount) = (char*)malloc(strlen(extension_name) + 1);
	memcpy(*(global_extensions + glfwExtensionCount), extension_name, strlen(extension_name) + 1);

	printf("Global extensions:\n");
	for(int i = 0; i < numExtensions; i++){
		printf("%s\n", *(global_extensions + i));
	}
	printf("----------------\n");

	createInfo.enabledExtensionCount = numExtensions;
	createInfo.ppEnabledExtensionNames = global_extensions;
	createInfo.enabledLayerCount = 0;

	// Validation Layers
	std::vector<const char*> layers = {
		"VK_LAYER_KHRONOS_validation",
	};

	uint32_t numValidationLayers;
	vkEnumerateInstanceLayerProperties(&numValidationLayers, nullptr);
	VkLayerProperties* layerProperties = (VkLayerProperties*)malloc(sizeof(VkLayerProperties) * numValidationLayers);
	vkEnumerateInstanceLayerProperties(&numValidationLayers, layerProperties);

	// Check if requested validation layers in *layers* are supported
	for(int j = 0; j < layers.size(); j++){
		bool supported = false;
		for(int i = 0; i < numValidationLayers; i++){
			if(!strcmp(layerProperties[i].layerName, layers[j])){
				supported = true;
			}
		}
		if(!supported){
			std::cout << "requested validation layer(s) not supported" << std::endl;
		}
	}

	if(enableValidationLayers){
		createInfo.enabledLayerCount = layers.size();
		createInfo.ppEnabledLayerNames = layers.data();
	}

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	VkExtensionProperties* extensions = new VkExtensionProperties[extensionCount];
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions);

	std::cout << extensionCount << " available extensions:\n";

	for(int i = 0; i < extensionCount; i++){
		std::cout << extensions[i].extensionName << std::endl;
	}

	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
	if(result != VK_SUCCESS){
		throw std::runtime_error("failed to create instance!");
	}

	free(layerProperties);
	free(*(global_extensions + glfwExtensionCount));
	free(global_extensions);
	delete[] extensions;
}

int main(){
	GLFWwindow* window = nullptr;
	VkInstance instance;

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(width, height, title, nullptr, nullptr);

	createInstance();

	while(!glfwWindowShouldClose(window)){
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}