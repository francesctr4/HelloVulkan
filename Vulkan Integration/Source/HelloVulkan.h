#pragma once

#include <iostream>
#include <vector> // Necessary for std::vector
#include <optional> // Necessary for std::optional (C++17)
#include <set> // Necessary for std::set
#include <cstdint> // Necessary for uint32_t
#include <limits> // Necessary for std::numeric_limits
#include <algorithm> // Necessary for std::clamp
#include <fstream> // Ncessary for reading files

#include "External/SDL2/include/SDL.h"
#include "External/SDL2/include/SDL_vulkan.h"
#pragma comment( lib, "Source/External/SDL2/libx64/SDL2.lib" )
#pragma comment( lib, "Source/External/SDL2/libx64/SDL2main.lib" )

#include "External/vulkan/vulkan.h"
#pragma comment (lib, "Source/External/vulkan/lib/vulkan-1.lib")

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
	void CreateGraphicsPipeline();
	void CreateFramebuffers();

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
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	std::vector<VkFramebuffer> swapChainFramebuffers;

};