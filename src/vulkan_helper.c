#include "vulkan_helper.h"

void init_vulkan(Vulkan* v){
	create_window(v);
	create_instance(v, 1);
	pick_physical_device(v);
	get_graphics_queue_family_index(v);
	create_logical_device(v);
	glfwCreateWindowSurface(v->instance, v->window, NULL, &v->surface);
	create_swapchain(v);
	create_image_views(v);
	create_render_pass(v);
	create_graphics_pipeline(v);
	create_framebuffers(v);
	create_command_pool(v);
	create_vertex_buffer(v);
	create_command_buffers(v);
	create_sync_objects(v);
}

uint32_t find_memory_type(Vulkan* v, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(v->physical_device, &memory_properties);
	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	err("Failed to find memory type");
	return 0;
}

void create_buffer(Vulkan* v, VkDeviceSize size, VkBufferUsageFlags usage_flags, uint32_t memory_flags, VkBuffer* buffer, VkDeviceMemory* memory){
	VkBufferCreateInfo create_info;
	create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	create_info.pNext = NULL;
	create_info.flags = 0;
	create_info.size = size;
	create_info.usage = usage_flags;
	create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	
	if(vkCreateBuffer(v->logical_device, &create_info, NULL, buffer) != VK_SUCCESS){
		err("Failed to create vertex buffer");
	}

	VkMemoryRequirements memory_requirements;
	vkGetBufferMemoryRequirements(v->logical_device, *buffer, &memory_requirements);

	VkMemoryAllocateInfo alloc_info;
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.allocationSize = memory_requirements.size;
	alloc_info.memoryTypeIndex = find_memory_type(v, memory_requirements.memoryTypeBits, memory_flags);
	alloc_info.pNext = NULL;

	if (vkAllocateMemory(v->logical_device, &alloc_info, NULL, memory) != VK_SUCCESS) {
		err("Failed to allocate memory for vertex buffer");
	}

	vkBindBufferMemory(v->logical_device, *buffer, *memory, 0);
}

void copy_buffer(Vulkan* v, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
	VkCommandBufferAllocateInfo alloc_info = {0};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = v->command_pool;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;
    vkAllocateCommandBuffers(v->logical_device, &alloc_info, &command_buffer);

	VkCommandBufferBeginInfo begin_info = {0};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(command_buffer, &begin_info);

	VkBufferCopy copy_region = {0};
	copy_region.srcOffset = 0; // Optional
	copy_region.dstOffset = 0; // Optional
	copy_region.size = size;
	vkCmdCopyBuffer(command_buffer, srcBuffer, dstBuffer, 1, &copy_region);

	vkEndCommandBuffer(command_buffer);

	VkSubmitInfo submit_info = {0};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;

	vkQueueSubmit(v->graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
	vkQueueWaitIdle(v->graphics_queue);

	vkFreeCommandBuffers(v->logical_device, v->command_pool, 1, &command_buffer);
}

void create_vertex_buffer(Vulkan* v){
	VkDeviceSize size = 3 * sizeof(float) * 5;
	VkBuffer staging_buffer;
	VkDeviceMemory staging_memory;
	create_buffer(v, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &staging_buffer, &staging_memory);
	
	// Uploading the data
	float vertices[] = {
		// Position          Color
		0.0f, 0.0f,          0.0f, 1.0f, 0.0f,
		1.0f, 0.0f,          1.0f, 0.0f, 0.0f,
		1.0f, 1.0f,          0.0f, 0.0f, 1.0f,
	};

	void* data;
	vkMapMemory(v->logical_device, staging_memory, 0, size, 0, &data);
	memcpy(data, vertices, size);
	vkUnmapMemory(v->logical_device, staging_memory);
	create_buffer(v, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &v->vertex_buffer, &v->vertex_buffer_memory);

	copy_buffer(v, staging_buffer, v->vertex_buffer, size);

    vkDestroyBuffer(v->logical_device, staging_buffer, NULL);
    vkFreeMemory(v->logical_device, staging_memory, NULL);
}

void create_window(Vulkan* v){
	if(glfwInit() != GLFW_TRUE){
		printf("Failed to initialize GLFW");
	};
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	v->window = glfwCreateWindow(800, 600, "Our Vulkan Window", NULL, NULL);
}

VkShaderModule createShaderModule(const VkDevice* device, file_buffer buffer) {
	VkShaderModuleCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = buffer.size;
	createInfo.pCode = (void*)buffer.data;
	createInfo.pNext = NULL;
	createInfo.flags = 0;

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(*device, &createInfo, NULL, &shaderModule) != VK_SUCCESS) {
		err("Failed to create shader module");
	}

	free_file(&buffer);

	return shaderModule;
}

void create_graphics_pipeline(Vulkan* v){
	file_buffer vertShaderData = read_file("res/vert.spv");
	file_buffer fragShaderData = read_file("res/frag.spv");

	// Wrapping the shader bytecode into a VkShaderModule wrapper object
	VkShaderModule vertShaderModule = createShaderModule(&v->logical_device, vertShaderData);
	VkShaderModule fragShaderModule = createShaderModule(&v->logical_device, fragShaderData);

	// Inserting the vertex shader into the pipeline
	VkPipelineShaderStageCreateInfo vertShaderStageInfo;
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";
	vertShaderStageInfo.pNext = NULL;
	vertShaderStageInfo.flags = 0;
	vertShaderStageInfo.pSpecializationInfo = NULL;

	// Inserting the fragment shader into the pipeline
	VkPipelineShaderStageCreateInfo fragShaderStageInfo;
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";
	fragShaderStageInfo.pNext = NULL;
	fragShaderStageInfo.flags = 0;
	fragShaderStageInfo.pSpecializationInfo = NULL;

	VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

	VkVertexInputBindingDescription binding_description;
	binding_description.stride = sizeof(float) * 5;
	binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	binding_description.binding = 0;

	VkVertexInputAttributeDescription bindings[2];
	bindings[0].offset = 0;
	bindings[0].location = 0;
	bindings[0].binding = 0;
	bindings[0].format = VK_FORMAT_R32G32_SFLOAT;

	bindings[1].offset = sizeof(float) * 2;
	bindings[1].location = 1;
	bindings[1].binding = 0;
	bindings[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	
	// Describing the vertex data
	VkPipelineVertexInputStateCreateInfo vertexInputInfo;
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &binding_description;
	vertexInputInfo.vertexAttributeDescriptionCount = 2;
	vertexInputInfo.pVertexAttributeDescriptions = bindings;
	vertexInputInfo.pNext = NULL;
	vertexInputInfo.flags = 0;

	// Setting the type of rendering(triangle strip, line, line strip etc)
	VkPipelineInputAssemblyStateCreateInfo inputAssembly;
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	inputAssembly.flags = 0;
	inputAssembly.pNext = NULL;

	// Setting up the viewport
	VkViewport viewport;
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float) v->capabilities.currentExtent.width;
	viewport.height = (float)v->capabilities.currentExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	// Setting up the scissors
	VkRect2D scissor;
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent = v->capabilities.currentExtent;

	// We must combine the viewport and scissor into 1 struct
	VkPipelineViewportStateCreateInfo viewportState;
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;
	viewportState.pNext = NULL;
	viewportState.flags = 0;


	// Setting up the rasterization stage
	VkPipelineRasterizationStateCreateInfo rasterizer;
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
	rasterizer.pNext = NULL;
	rasterizer.flags = 0;

	// Setting up the color blending from the fragment shader and framebuffer
	VkPipelineMultisampleStateCreateInfo multisampling;
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = NULL; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional
	multisampling.pNext = VK_FALSE; // Optional
	multisampling.flags = 0; // Optional

	// Color blending parameters per framebuffer
	VkPipelineColorBlendAttachmentState colorBlendAttachment;
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	VkPipelineColorBlendStateCreateInfo colorBlending;
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional
	colorBlending.pNext = NULL;
	colorBlending.flags = 0;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo;
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0; // Optional
	pipelineLayoutInfo.pSetLayouts = NULL; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = NULL; // Optional
	pipelineLayoutInfo.pNext = NULL; // Optional
	pipelineLayoutInfo.flags = 0; // Optional

	if (vkCreatePipelineLayout(v->logical_device, &pipelineLayoutInfo, NULL, &v->pipeline_layout) != VK_SUCCESS) {
		err("Failed to create pipeline layout");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo;
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = NULL; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = NULL; // Optional
	pipelineInfo.layout = v->pipeline_layout;
	pipelineInfo.renderPass = v->render_pass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional
	pipelineInfo.pNext = NULL;
	pipelineInfo.flags = 0;

	// Finally creating the actual pipeline
	if (vkCreateGraphicsPipelines(v->logical_device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &v->graphics_pipeline) != VK_SUCCESS) {
		err("Failed to create graphics pipeline");
	}

	// Don't need the shaders after pipeline creation is complete
	vkDestroyShaderModule(v->logical_device, fragShaderModule, NULL);
	vkDestroyShaderModule(v->logical_device, vertShaderModule, NULL);
}

void pick_physical_device(Vulkan* v){
	uint32_t physical_device_count;
	vkEnumeratePhysicalDevices(v->instance, &physical_device_count, NULL);
	VkPhysicalDevice* physical_devices = malloc(sizeof(VkPhysicalDeviceProperties) * physical_device_count);
	vkEnumeratePhysicalDevices(v->instance, &physical_device_count, physical_devices);

	if(physical_device_count == 0){
		err("Error: Failed to get physical device");
	}

	v->physical_device = physical_devices[0];
	free(physical_devices);
}

void get_graphics_queue_family_index(Vulkan* v){
	uint32_t queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(v->physical_device, &queue_family_count, NULL);
	VkQueueFamilyProperties* queue_families = malloc(sizeof(VkQueueFamilyProperties) * queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(v->physical_device, &queue_family_count, queue_families);
	if(queue_family_count == 0) {
		err("Found no queue families in physical device");
	}

	for(uint32_t i = 0; i < queue_family_count; i++){
		if(queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT){
			v->graphics_queue_index = i;
		}
	}
	
	free(queue_families);
}

void draw_frame(Vulkan* v){
	vkWaitForFences(v->logical_device, 1, &v->in_flight_fences[v->current_frame], VK_TRUE, UINT64_MAX);
	vkResetFences(v->logical_device, 1, &v->in_flight_fences[v->current_frame]);
	
	uint32_t imageIndex;
	VkResult result;
	result = vkAcquireNextImageKHR(v->logical_device, v->swapchain, UINT64_MAX, v->image_available_semaphores[v->current_frame], VK_NULL_HANDLE, &imageIndex);
	if(result != VK_SUCCESS){
		err("Failed to aquire swapchain image");
	}

	VkSubmitInfo submitInfo;
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &v->command_buffers[imageIndex];
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &v->image_available_semaphores[v->current_frame];
	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	submitInfo.pWaitDstStageMask = &waitStage;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &v->render_finished_semaphores[v->current_frame];
	submitInfo.pNext = NULL;

	if (vkQueueSubmit(v->graphics_queue, 1, &submitInfo, v->in_flight_fences[v->current_frame]) != VK_SUCCESS) {
		err("Failed to submit to graphics queue");
	}

	VkPresentInfoKHR presentInfo;
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &v->render_finished_semaphores[v->current_frame];
	presentInfo.swapchainCount = 1;
	VkSwapchainKHR swapChains[] = {v->swapchain};
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = NULL;
	presentInfo.pNext = NULL;

	result = vkQueuePresentKHR(v->graphics_queue, &presentInfo);
	if(result != VK_SUCCESS){
		err("Failed to aquire swapchain image");
	}

	v->current_frame = (v->current_frame + 1) % v->num_image_views;
}

void create_sync_objects(Vulkan* v){
	v->current_frame = 0;
	v->image_available_semaphores = malloc(sizeof(VkSemaphore) * v->num_image_views);
	v->render_finished_semaphores = malloc(sizeof(VkSemaphore) * v->num_image_views);
	v->in_flight_fences = malloc(sizeof(VkFence) * v->num_image_views);

	VkSemaphoreCreateInfo semaphoreInfo;
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreInfo.pNext = NULL;
	semaphoreInfo.flags = 0;

	VkFenceCreateInfo fenceInfo;
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.pNext = NULL;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for(int i = 0; i < v->num_image_views; i++){
		if (vkCreateSemaphore(v->logical_device, &semaphoreInfo, NULL, &v->image_available_semaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(v->logical_device, &semaphoreInfo, NULL, &v->render_finished_semaphores[i]) != VK_SUCCESS ||
			vkCreateFence(v->logical_device, &fenceInfo, NULL, &v->in_flight_fences[i]) != VK_SUCCESS) {
			err("Failed to create sync objects");
		}
	}
}

void create_logical_device(Vulkan* v){
	float priority = 1.0f;
	VkDeviceQueueCreateInfo queue_create_info;
	queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_create_info.queueFamilyIndex = v->graphics_queue_index;
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

	if(vkCreateDevice(v->physical_device, &device_create_info, NULL, &v->logical_device) != VK_SUCCESS){
		err("Failed to create device");
	}

	vkGetDeviceQueue(v->logical_device, v->graphics_queue_index, 0, &v->graphics_queue);
}

void create_instance(Vulkan* v, uint8_t validation_layers){
	VkInstanceCreateInfo create_info;
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = NULL;

	// Copying GLFW extensions as well as our custom extensions into a pp extension names array
	uint32_t glfw_extension_count = 0;
	const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
	if(glfw_extensions == NULL){
		err("Failed to get glfw extensions");
	}

	uint32_t total_extension_count = glfw_extension_count + 1;
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

	if(vkCreateInstance(&create_info, NULL, &v->instance) != VK_SUCCESS){
		err("Failed to create instance");
	}

	free(total_extensions);
}

void create_swapchain(Vulkan* v){
	// Get surface capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(v->physical_device, v->surface, &v->capabilities);

	// Get supported swapchain image formats
	uint32_t format_count = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(v->physical_device, v->surface, &format_count, NULL);
	VkSurfaceFormatKHR* formats = malloc(sizeof(VkSurfaceFormatKHR) * format_count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(v->physical_device, v->surface, &format_count, formats);

	// Picking the right surface format
	VkSurfaceFormatKHR format;
	for(uint32_t i = 0; i < format_count; i++){
		if(formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR){
			format = formats[i];
		}
	}

	VkExtent2D extent = v->capabilities.currentExtent;
	VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;

	uint32_t image_count = v->capabilities.minImageCount + 1;
	if(v->capabilities.maxImageCount > 0 && image_count > v->capabilities.maxImageCount){
		image_count = v->capabilities.maxImageCount;
	}

	// Populate swapchain creation struct
	VkSwapchainCreateInfoKHR swapchain_create_info;
	swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchain_create_info.surface = v->surface;
	swapchain_create_info.minImageCount = image_count;
	swapchain_create_info.imageExtent = extent;
	swapchain_create_info.imageFormat = format.format;
	swapchain_create_info.imageColorSpace = format.colorSpace;
	swapchain_create_info.imageArrayLayers = 1;
	swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchain_create_info.preTransform = v->capabilities.currentTransform;
	swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchain_create_info.presentMode = present_mode;
	swapchain_create_info.clipped = VK_TRUE;
	swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;
	swapchain_create_info.pNext = NULL;
	swapchain_create_info.flags = 0;

	if(vkCreateSwapchainKHR(v->logical_device, &swapchain_create_info, NULL, &v->swapchain) != VK_SUCCESS){
		err("Failed to create swapchain");
	}

	free(formats);
}

void create_image_views(Vulkan* v){
	uint32_t image_count;
	vkGetSwapchainImagesKHR(v->logical_device, v->swapchain, &image_count, NULL);
	VkImage* images = malloc(sizeof(VkImage) * image_count);
	vkGetSwapchainImagesKHR(v->logical_device, v->swapchain, &image_count, images);
	v->num_image_views = image_count;

	v->image_views = malloc(sizeof(VkImageView) * image_count);

	for(uint32_t i = 0; i < image_count; i++){
		VkImageViewCreateInfo create_info;
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.image = images[i];
		create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		create_info.format = VK_FORMAT_B8G8R8A8_SRGB;
		create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;
		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.layerCount = 1;
		create_info.pNext = NULL;
		create_info.flags = 0;

		if(vkCreateImageView(v->logical_device, &create_info, NULL, &v->image_views[i]) != VK_SUCCESS){
			err("Falied creating image view");
		}
	}

	free(images);
}

void create_render_pass(Vulkan* v){
	VkAttachmentDescription colorAttachment;
	colorAttachment.format = VK_FORMAT_B8G8R8A8_SRGB;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	colorAttachment.flags = 0;

	VkAttachmentReference colorAttachmentRef;
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass;
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = NULL;
	subpass.flags = 0;
	subpass.inputAttachmentCount = 0;
	subpass.pInputAttachments = NULL;
	subpass.pResolveAttachments = NULL;
	subpass.preserveAttachmentCount = 0;
	subpass.pPreserveAttachments = NULL;

	VkSubpassDependency dependency;
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependency.dependencyFlags = 0;

	VkRenderPassCreateInfo renderPassInfo;
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;
	renderPassInfo.pNext = NULL;
	renderPassInfo.flags = 0;

	if (vkCreateRenderPass(v->logical_device, &renderPassInfo, NULL, &v->render_pass) != VK_SUCCESS) {
		err("Failed to create render pass");
	}
}

void create_framebuffers(Vulkan* v){
	v->framebuffers = malloc(sizeof(VkFramebuffer) * v->num_image_views);

	for (int i = 0; i < v->num_image_views; i++) {
		VkImageView attachments[] = {
			v->image_views[i]
		};

		VkFramebufferCreateInfo framebufferInfo;
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = v->render_pass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = v->capabilities.currentExtent.width;
		framebufferInfo.height = v->capabilities.currentExtent.height;
		framebufferInfo.layers = 1;
		framebufferInfo.pNext = NULL;
		framebufferInfo.flags = 0;

		if (vkCreateFramebuffer(v->logical_device, &framebufferInfo, NULL, &(v->framebuffers)[i]) != VK_SUCCESS) {
			err("Failed to create framebuffer");
		}
	}
}

void create_command_pool(Vulkan* v){
	VkCommandPoolCreateInfo poolInfo;
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = v->graphics_queue_index;
	poolInfo.flags = 0; // Optional
	poolInfo.pNext = NULL;

	if (vkCreateCommandPool(v->logical_device, &poolInfo, NULL, &v->command_pool) != VK_SUCCESS) {
		err("Failed to create command pool");
	}
}

void create_command_buffers(Vulkan* v) {
	// Creating the command buffers
	v->command_buffers = malloc(sizeof(VkCommandBuffer) * v->num_image_views);
	VkCommandBufferAllocateInfo allocInfo;
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = v->command_pool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = v->num_image_views;
	allocInfo.pNext = NULL;

	if (vkAllocateCommandBuffers(v->logical_device, &allocInfo, v->command_buffers) != VK_SUCCESS) {
		err("Failed to allocate command buffer");
	}

	// Recording data into the command buffers
	for (size_t i = 0; i < v->num_image_views; i++) {
		VkCommandBufferBeginInfo beginInfo;
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = NULL; // Optional
		beginInfo.pNext = NULL;

		if (vkBeginCommandBuffer(v->command_buffers[i], &beginInfo) != VK_SUCCESS) {
			err("Failed to begin recording command buffer");
		}

		VkRenderPassBeginInfo renderPassInfo;
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = v->render_pass;
		renderPassInfo.framebuffer = v->framebuffers[i];
		renderPassInfo.renderArea.offset.x = 0;
		renderPassInfo.renderArea.offset.y = 0;
		renderPassInfo.renderArea.extent = v->capabilities.currentExtent;
		VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;
		renderPassInfo.pNext = NULL;

		vkCmdBeginRenderPass(v->command_buffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(v->command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, v->graphics_pipeline);
		
		VkDeviceSize offset = {0};
		vkCmdBindVertexBuffers(v->command_buffers[i], 0, 1, &v->vertex_buffer, &offset);

		vkCmdDraw(v->command_buffers[i], 3, 1, 0, 0);

		vkCmdEndRenderPass(v->command_buffers[i]);

		if (vkEndCommandBuffer(v->command_buffers[i]) != VK_SUCCESS) {
			err("Failed to record command buffer");
		}
	}
}

void destroy_vulkan(Vulkan* v){
	vkDeviceWaitIdle(v->logical_device);

	vkDestroyBuffer(v->logical_device, v->vertex_buffer, NULL);
	vkFreeMemory(v->logical_device, v->vertex_buffer_memory, NULL);

	for (int i = 0; i < v->num_image_views; i++) {
		vkDestroyFramebuffer(v->logical_device, v->framebuffers[i], NULL);
	}

	vkDestroyCommandPool(v->logical_device, v->command_pool, NULL);

	vkDestroyRenderPass(v->logical_device, v->render_pass, NULL);

	vkDestroyPipeline(v->logical_device, v->graphics_pipeline, NULL);

	vkDestroyPipelineLayout(v->logical_device, v->pipeline_layout, NULL);

	for (int i = 0; i < v->num_image_views; i++) {
		vkDestroyImageView(v->logical_device, v->image_views[i], NULL);
	}

	vkDestroySwapchainKHR(v->logical_device, v->swapchain, NULL);

	for (int i = 0; i < v->num_image_views; i++) {
		vkDestroySemaphore(v->logical_device, v->image_available_semaphores[i], NULL);
		vkDestroySemaphore(v->logical_device, v->render_finished_semaphores[i], NULL);
		vkDestroyFence(v->logical_device, v->in_flight_fences[i], NULL);
	}

	vkDestroyDevice(v->logical_device, NULL);

	vkDestroySurfaceKHR(v->instance, v->surface, NULL);

	vkDestroyInstance(v->instance, NULL);

	glfwDestroyWindow(v->window);
	glfwTerminate();
}
