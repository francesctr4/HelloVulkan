#pragma once

#include <iostream>
#include <vector>

#include "SDL2/include/SDL.h"
#include "SDL2/include/SDL_vulkan.h"
#pragma comment( lib, "SDL2/libx64/SDL2.lib" )
#pragma comment( lib, "SDL2/libx64/SDL2main.lib" )

#include "vulkan/vulkan.h"
#pragma comment (lib, "vulkan/lib/vulkan-1.lib")

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