#include <iostream>

#include "SDL2/include/SDL.h"
#pragma comment( lib, "SDL2/libx64/SDL2.lib" )
#pragma comment( lib, "SDL2/libx64/SDL2main.lib" )

#include "vulkan/vulkan.h"
#include "SDL2/include/SDL_vulkan.h"

#include "Globals.h"

int main(int argc, char** argv) {

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    
    // Create a window
    SDL_Window* window = SDL_CreateWindow(gAppName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, gWindowWidth, gWindowHeight, SDL_WINDOW_SHOWN);

    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    // Main event loop
    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
    }

    // Destroy the window and quit SDL
    SDL_DestroyWindow(window);
    SDL_Quit();

	return 0;
}