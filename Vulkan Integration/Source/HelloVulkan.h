#pragma once

// Global Includes
#include <iostream>			// Necessary for std::cout / std::cin
#include <vector>			// Necessary for std::vector
#include <optional>			// Necessary for std::optional (C++17)
#include <set>				// Necessary for std::set
#include <cstdint>			// Necessary for uint32_t
#include <limits>			// Necessary for std::numeric_limits
#include <algorithm>		// Necessary for std::clamp
#include <fstream>			// Necessary for reading files
#include <array>			// Necessary for std::array
#include <chrono>			// Necessary for std::chrono (time management)
#include <unordered_map>	// Necessary for std::unordered_map

// Library Includes
#include "SDL2.h"
#include "Vulkan.h"
#include "glmath.h"
#include "Assimp.h"

const int MAX_FRAMES_IN_FLIGHT = 2;

// --------------- Validation Layers ---------------

const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

#ifdef _DEBUG
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = false;
#endif

// --------------- Debug Messenger ---------------

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const
	VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const
	VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT*
	pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
	VkDebugUtilsMessengerEXT debugMessenger, const
	VkAllocationCallbacks* pAllocator);

// --------------- Logical Device Creation ---------------

struct QueueFamilyIndices {

	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool IsComplete() {

		return graphicsFamily.has_value() && presentFamily.has_value();

	}

};

// --------------- Swap Chain Creation ---------------

struct SwapChainSupportDetails {

	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
	VkSurfaceCapabilitiesKHR capabilities;

};

// --------------- Vertex Struct ---------------

struct Vertex {

	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 texCoord;

	static VkVertexInputBindingDescription GetBindingDescription() {

		VkVertexInputBindingDescription bindingDescription{};

		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;

	}

	static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions() {

		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

		// Position

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		// Color

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		// Texture Coordinates

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

		return attributeDescriptions;

	}

	bool operator==(const Vertex& other) const {
		
		return position == other.position && color == other.color && texCoord == other.texCoord;

	}

};

// --------------- Uniform Buffer Object Struct (UBO) ---------------

struct UniformBufferObject {
	
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 projection;
	
};

// --------------- Vulkan App Class ---------------

class HelloVulkan {
public:

	void Run();

private:

	// --------------- Core Functions (separated by topic) ---------------

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

	void CreateColorResources();
	void CreateDepthResources();

	void CreateTextureImage();
	void CreateTextureImageView();
	void CreateTextureSampler();

	void LoadModel();

	void CreateVertexBuffer();
	void CreateIndexBuffer();
	void CreateUniformBuffers();

	void CreateDescriptorPool();
	void CreateDescriptorSets();

	void CreateCommandBuffers();
	void CreateSyncObjects();

	void Update();

	void DrawFrame();

	void RecreateSwapChain();
	void CleanUpSwapChain();

	void CleanUp();

	// ------------------------------------ Helper Functions ------------------------------------

	// --------------- Validation Layers ---------------

	bool CheckValidationLayerSupport(const std::vector<const char*>& validationLayers);

	// --------------- Vulkan Extensions ---------------

	void ShowSupportedExtensions();
	std::vector<const char*> GetRequiredExtensions();

	// --------------- Validation Layers Debug Messenger ---------------

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType, 
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
		void* pUserData);

	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	// --------------- Swap Chain ---------------

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	// --------------- Vulkan Physical Device ---------------

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

	bool IsPhysicalDeviceSuitable(VkPhysicalDevice device);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

	// --------------- Shader Modules ---------------

	static std::vector<char> ReadFile(const std::string& filename);
	VkShaderModule CreateShaderModule(const std::vector<char>& code);

	// --------------- Command Buffers ---------------

	VkCommandBuffer BeginSingleTimeCommands();
	void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

	void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

	// --------------- Buffer Creation Utils (e.g. Vertex Buffers) --------------- 

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties, VkBuffer& buffer,
		VkDeviceMemory& bufferMemory);

	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	// --------------- Uniform Buffers ---------------

	void UpdateUniformBuffer(uint32_t currentImage);

	// --------------- Texture Mapping ---------------

	void CreateImage(uint32_t width, uint32_t height,
		uint32_t mipLevels, VkSampleCountFlagBits numSamples,
		VkFormat format, VkImageTiling tiling,
		VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
		VkImage& image, VkDeviceMemory& imageMemory);

	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

	void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

	// --------------- Depth Buffering ---------------

	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates,
		VkImageTiling tiling,
		VkFormatFeatureFlags features);

	VkFormat FindDepthFormat();

	bool HasStencilComponent(VkFormat format);

	// --------------- Loading Models ---------------

	void ProcessNode(aiNode* node, const aiScene* scene);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene);

	// --------------- Generating Mipmaps ---------------

	void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

	// --------------- Multisampling ---------------

	VkSampleCountFlagBits GetMaxUsableSampleCount();

private:

	// ------------------------------ CORE ELEMENTS ------------------------------

	// --------------- SDL Window ---------------

	SDL_Window* window;

	// --------------- Vulkan Instance and Surface ---------------

	VkInstance instance;
	VkSurfaceKHR surface;

	// --------------- Validation Layers Debug Messenger ---------------

	VkDebugUtilsMessengerEXT debugMessenger;

	// --------------- Vulkan Physical Device ---------------
	
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

	// --------------- Vulkan Logical Device ---------------

	VkDevice logicalDevice;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	// --------------- Swap Chain ---------------

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	// --------------- Graphics Pipeline ---------------

	VkRenderPass renderPass;
	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	// --------------- Frame Buffers ---------------

	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;

	// --------------- Command Buffers ---------------

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;
	
	// --------------- Synchronization Objects ---------------

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;

	uint32_t currentFrame = 0;

	// --------------- Resize Handling ---------------

	bool framebufferResized = false;

	// --------------- Vertex Buffers ---------------

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	// --------------- Uniform Buffers ---------------

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;

	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

	// --------------- Texture Mapping ---------------

	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;

	// --------------- Depth Buffering --------------- 

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	// --------------- Loading Models --------------- 

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	// --------------- Generating Mipmaps --------------- 

	uint32_t mipLevels;

	// --------------- Multisampling --------------- 

	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

	VkImage colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView colorImageView;

};