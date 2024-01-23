#pragma once

// Global Includes
#include <iostream>		// Necessary for std::cout / std::cin
#include <vector>		// Necessary for std::vector
#include <optional>		// Necessary for std::optional (C++17)
#include <set>			// Necessary for std::set
#include <cstdint>		// Necessary for uint32_t
#include <limits>		// Necessary for std::numeric_limits
#include <algorithm>	// Necessary for std::clamp
#include <fstream>		// Necessary for reading files
#include <array>		// Necessary for std::array
#include <chrono>

// Library Includes
#include "SDL2.h"
#include "Vulkan.h"
#include "MathGeoLib.h"
#include "glmath.h"

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

#ifdef _DEBUG
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = false;
#endif

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const
	VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const
	VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT*
	pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
	VkDebugUtilsMessengerEXT debugMessenger, const
	VkAllocationCallbacks* pAllocator);

struct QueueFamilyIndices {

	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool IsComplete() { 

		return graphicsFamily.has_value() && presentFamily.has_value();

	}

};

struct SwapChainSupportDetails {

	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
	VkSurfaceCapabilitiesKHR capabilities;

};

struct Vertex {

	glm::vec2 position;
	glm::vec3 color;

	static VkVertexInputBindingDescription getBindingDescription() {

		VkVertexInputBindingDescription bindingDescription{};

		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;

	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {

		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

		// Position

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		// Color

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;

	}

};

struct UniformBufferObject {
	
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 projection;
	
};

class HelloVulkan {
public:

	void Run();

private:

	bool InitSDLWindow();

	void InitVulkan();
	bool CreateInstance();
	void CreateSurface();
	void SetupDebugMessenger();
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	void CreateSwapChain();
	void CreateImageViews();
	void CreateRenderPass();
	void CreateDescriptorSetLayout();
	void CreateGraphicsPipeline();
	void CreateFramebuffers();
	void CreateCommandPool();
	void CreateVertexBuffer();
	void CreateIndexBuffer();
	void CreateUniformBuffers();
	void CreateDescriptorPool();
	void CreateDescriptorSets();
	void CreateCommandBuffers();
	void CreateSyncObjects();

	void RecreateSwapChain();
	void CleanUpSwapChain();

	void UpdateUniformBuffer(uint32_t currentImage);

	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, 
	                  VkMemoryPropertyFlags properties, VkBuffer& buffer, 
					  VkDeviceMemory& bufferMemory);

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

	static std::vector<char> ReadFile(const std::string& filename);
	VkShaderModule CreateShaderModule(const std::vector<char>& code);
	
	bool IsPhysicalDeviceSuitable(VkPhysicalDevice device);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	bool CheckValidationLayerSupport(const std::vector<const char*>& validationLayers);

	void ShowSupportedExtensions();
	std::vector<const char*> GetRequiredExtensions();

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType, 
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
		void* pUserData);

	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	void DrawFrame();

	void Update();
	void CleanUp();

private:

	SDL_Window* window;

	VkInstance instance;
	VkSurfaceKHR surface;

	VkDebugUtilsMessengerEXT debugMessenger;
	
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice logicalDevice;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	std::vector<VkImageView> swapChainImageViews;

	VkRenderPass renderPass;
	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	std::vector<VkFramebuffer> swapChainFramebuffers;

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	uint32_t currentFrame = 0;

	bool framebufferResized = false;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;

	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

};