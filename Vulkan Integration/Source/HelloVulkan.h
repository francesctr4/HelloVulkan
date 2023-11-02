#pragma once

#include <iostream>
#include <vector>
#include <optional>

#include "External/SDL2/include/SDL.h"
#include "External/SDL2/include/SDL_vulkan.h"
#pragma comment( lib, "Source/External/SDL2/libx64/SDL2.lib" )
#pragma comment( lib, "Source/External/SDL2/libx64/SDL2main.lib" )

#include "External/vulkan/vulkan.h"
#pragma comment (lib, "Source/External/vulkan/lib/vulkan-1.lib")

#ifdef _DEBUG
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = false;
#endif

struct QueueFamilyIndices {

	std::optional<uint32_t> graphicsFamily;

	bool IsComplete() { 

		return graphicsFamily.has_value(); 

	}

};

class HelloVulkan {
public:

	void Run();

private:

	bool InitSDLWindow();

	void InitVulkan();
	bool CreateInstance();
	void SetupDebugMessenger();
	void PickPhysicalDevice();
	bool IsDeviceSuitable(VkPhysicalDevice device);
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

	bool CheckValidationLayerSupport(const std::vector<const char*>& validationLayers);

	void ShowSupportedExtensions();
	std::vector<const char*> GetRequiredExtensions();

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType, 
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
		void* pUserData);

	static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const
		VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const
		VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT*
		pDebugMessenger);

	static void DestroyDebugUtilsMessengerEXT(VkInstance instance,
		VkDebugUtilsMessengerEXT debugMessenger, const
		VkAllocationCallbacks* pAllocator);

	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	void Update();
	void CleanUp();

private:

	SDL_Window* window;
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

	//VkSurfaceKHR surface;

};