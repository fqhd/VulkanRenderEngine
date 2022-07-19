CC="gcc"
GLFW="-I/opt/homebrew/Cellar/glfw/3.3.6/include -L/opt/homebrew/Cellar/glfw/3.3.6/lib -lglfw"
CGLM="-I/opt/homebrew/Cellar/cglm/0.8.4/include"
VULKAN="-L/Users/fahd/VulkanSDK/1.2.198.1/macOS/lib -lvulkan -I/Users/fahd/VulkanSDK/1.2.198.1/macOS/include"

$CC $GLFW $CGLM $VULKAN src/main.c src/vulkan_helper.c src/utils.c src/renderer.c
./a.out