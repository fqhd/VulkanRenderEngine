CC = gcc
GLFW = -L/opt/homebrew/Cellar/glfw/3.3.6/lib -lglfw -I/opt/homebrew/Cellar/glfw/3.3.6/include
VULKAN = -L/Users/fahd/VulkanSDK/1.2.198.1/macOS/lib -lvulkan -I/Users/fahd/VulkanSDK/1.2.198.1/macOS/include
CGLM = -I/opt/homebrew/Cellar/cglm/0.8.4/include

out: main.c
	${CC} main.c -std=c99 ${GLFW} ${VULKAN} ${CGLM}
