#pragma once

// Global Settings

#define MODELS_DIRECTORY "Assets/Models/"
#define TEXTURES_DIRECTORY "Assets/Textures/"
#define SHADERS_DIRECTORY "Assets/Shaders/"

const std::string appName = "VulkanDemo";
const std::string engineName = "VulkanDemoEngine";

const uint32_t windowWidth = 800;
const uint32_t windowHeight = 600;

const std::string modelPath = MODELS_DIRECTORY + std::string("Viking_Room.obj");
const std::string texturePath = TEXTURES_DIRECTORY + std::string("Viking_Room.png");

//static const std::vector<Vertex> vertices = {
//
//    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
//    {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
//    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
//    {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
//
//    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
//    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
//    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
//    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
//
//};
//
//static const std::vector<uint16_t> indices = {
//
//    0, 1, 2,
//    2, 3, 0,
//
//    4, 5, 6,
//    6, 7, 4
//
//};

// Global Math Operations

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define PI 3.14159265358979323846264338327950288