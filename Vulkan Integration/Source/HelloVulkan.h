#pragma once

#include <iostream>
#include <vector>

#include "External/SDL2/include/SDL.h"
#include "External/SDL2/include/SDL_vulkan.h"
#pragma comment( lib, "Source/External/SDL2/libx64/SDL2.lib" )
#pragma comment( lib, "Source/External/SDL2/libx64/SDL2main.lib" )

#include "External/vulkan/vulkan.h"
#pragma comment (lib, "Source/External/vulkan/lib/vulkan-1.lib")

class HelloVulkan {
public:

	void Run();

private:

	bool InitSDLWindow();
	void InitVulkan();
	bool CreateInstance();
	void Update();
	void CleanUp();

private:

	SDL_Window* window;
	VkInstance instance;
	//VkSurfaceKHR surface;

};