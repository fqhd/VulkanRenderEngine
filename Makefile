CC = clang
GLFW = $$(pkg-config --libs glfw3) $$(pkg-config --cflags glfw3)
VULKAN = -L/Users/fahd/VulkanSDK/1.2.198.1/macOS/lib -lvulkan -I/Users/fahd/VulkanSDK/1.2.198.1/macOS/include
CGLM = -I/opt/homebrew/Cellar/cglm/0.8.4/include

out: main.c
	${CC} main.c -std=c99 ${GLFW} ${VULKAN} ${CGLM} -o out
