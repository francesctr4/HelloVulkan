#include "HelloVulkan.h"

#include "Globals.h"

void HelloVulkan::Run()
{
    InitSDLWindow();
    InitVulkan();
    Update();
    CleanUp();
}

bool HelloVulkan::InitSDLWindow()
{
    bool ret = true;

    // Initialize SDL

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {

        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());

        ret = false;
    }

    // Create a window

    window = SDL_CreateWindow(gAppName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, gWindowWidth, gWindowHeight, SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);

    if (window == NULL) {

        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());

        ret = false;
    }

    return ret;
}

void HelloVulkan::InitVulkan()
{
    CreateInstance();
    SetupDebugMessenger();
    CreateSurface();
    PickPhysicalDevice();
    CreateLogicalDevice();
    CreateSwapChain();
    CreateImageViews();
}

bool HelloVulkan::CreateInstance()
{
    bool ret = true;

    if (enableValidationLayers && !CheckValidationLayerSupport(validationLayers)) {

        std::cout << "Validation Layers Requested, but not available!" << std::endl;

    }

    //ShowSupportedExtensions();

    std::vector<const char*> extensions = GetRequiredExtensions();

    VkApplicationInfo appInfo = {};

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = gAppName;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = gEngineName;
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledLayerCount = enableValidationLayers ? static_cast<uint32_t>(validationLayers.size()) : 0;
    createInfo.ppEnabledLayerNames = enableValidationLayers ? validationLayers.data() : nullptr;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

    if (enableValidationLayers) PopulateDebugMessengerCreateInfo(debugCreateInfo);

    createInfo.pNext = enableValidationLayers ? (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo : nullptr;

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

    switch (result)
    {
    case VK_SUCCESS:

        std::cout << "Vulkan instance created successfully.\n";

        break;

    case VK_ERROR_INCOMPATIBLE_DRIVER:

        std::cout << "Unable to create Vulkan Instance: cannot find a compatible Vulkan ICD.\n";

        ret = false;

        break;

    default:

        std::cout << "Unable to create Vulkan Instance: unknown error.\n";

        ret = false;

        break;
    }

    return ret;
}

void HelloVulkan::CreateSurface()
{
    if (!SDL_Vulkan_CreateSurface(window, instance, &surface)) {

        std::cout << "Failed to create Vulkan surface!" << std::endl;

    }
    else {

        std::cout << "Vulkan surface created successfully." << std::endl;

    }
}

void HelloVulkan::SetupDebugMessenger()
{
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};

    PopulateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {

        std::cout << "Failed to Set Up Debug Messenger!" << std::endl;

    }
}

void HelloVulkan::PickPhysicalDevice()
{
    uint32_t deviceCount = 0;

    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {

        std::cout << "Failed to find GPUs with Vulkan support!" << std::endl;
            
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);

    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const auto& device : devices) {

        if (IsPhysicalDeviceSuitable(device)) {

            physicalDevice = device;

            break;
            
        }
        
    }

    if (physicalDevice == VK_NULL_HANDLE) {

        std::cout << "Failed to find a suitable GPU!" << std::endl;
        
    }
    else {

        std::cout << "Suitable GPU found!" << std::endl;

    }

}

void HelloVulkan::CreateLogicalDevice()
{
    QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);
    
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    float queuePriority = 1.0f;

    for (uint32_t queueFamily : uniqueQueueFamilies) {

        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        queueCreateInfos.push_back(queueCreateInfo);
        
    }

    // Specifying used device features

    VkPhysicalDeviceFeatures deviceFeatures{};
    // [...]

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;
    
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    createInfo.enabledLayerCount = enableValidationLayers ? static_cast<uint32_t>(validationLayers.size()) : 0;
    createInfo.ppEnabledLayerNames = enableValidationLayers ? validationLayers.data() : nullptr;

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
        
        std::cout << "Failed to create logical device!\n";

    }
    else {

        std::cout << "Logical Device created successfully!\n";

    }

    vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentQueue);

}

void HelloVulkan::CreateSwapChain()
{
    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physicalDevice);
    
    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {

        imageCount = swapChainSupport.capabilities.maxImageCount;
        
    }

    VkSwapchainCreateInfoKHR createInfo{};

    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);

    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
    
    if (indices.graphicsFamily != indices.presentFamily) {

        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
        
    }
    else {

        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; 
        createInfo.pQueueFamilyIndices = nullptr; 
        
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
        
        std::cout << "Failed to create Swap Chain!" << std::endl;
        
    }
    else {

        std::cout << "Swap Chain created successfully!" << std::endl;

    }

    vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);

    vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}

void HelloVulkan::CreateImageViews()
{
    swapChainImageViews.resize(swapChainImages.size());


}

bool HelloVulkan::IsPhysicalDeviceSuitable(VkPhysicalDevice device)
{
    bool ret = false;

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    QueueFamilyIndices deviceIndices = FindQueueFamilies(device);

    bool extensionsSupported = CheckDeviceExtensionSupport(device);

    bool swapChainAdequate = false;

    if (extensionsSupported) {

        SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        
    }

    ret = deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
        deviceFeatures.geometryShader &&
        deviceIndices.IsComplete() &&
        extensionsSupported && 
        swapChainAdequate;

    if (ret) {

        std::cout << "Using GPU: ";
        std::cout << deviceProperties.deviceName << std::endl;

    }
    
    return ret;
}

bool HelloVulkan::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
    
    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
    
    for (const auto& extension : availableExtensions) {
        
        requiredExtensions.erase(extension.extensionName);
        
    }
    
    return requiredExtensions.empty();
}

QueueFamilyIndices HelloVulkan::FindQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    // Assign index to queue families that could be found

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {

        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {

            indices.graphicsFamily = i;
            
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (presentSupport) {
            
            indices.presentFamily = i;
           
        }

        if (indices.IsComplete()) {

            break;
 
        }

        i++;

    }

    return indices;
}

SwapChainSupportDetails HelloVulkan::QuerySwapChainSupport(VkPhysicalDevice device)
{
    SwapChainSupportDetails details;

    // Device Formats

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    
    if (formatCount != 0) {

        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
       
    }

    // Device Present Modes

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
    
    if (presentModeCount != 0) {
        
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        
    }

    // Device Capabilities

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
    
    return details;
}

VkSurfaceFormatKHR HelloVulkan::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats) {

        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && 
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {

            return availableFormat;
            
        }
        
    }

    return availableFormats[0];
}

VkPresentModeKHR HelloVulkan::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes) {
        
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) { 
            
            return availablePresentMode; // Triple Buffering
                
        }
            
    }
    
    return VK_PRESENT_MODE_FIFO_KHR; // Vertical Sync
}

VkExtent2D HelloVulkan::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {

        return capabilities.currentExtent;

    }
    else {
        
        int width, height;
        SDL_Vulkan_GetDrawableSize(window, &width, &height);
       
        VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
       
        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        
        return actualExtent;
        
    }
}

bool HelloVulkan::CheckValidationLayerSupport(const std::vector<const char*>& validationLayers)
{
    bool ret = true;

    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        
        bool layerFound = false;
        
        for (const auto& layerProperties : availableLayers) {

            if (strcmp(layerName, layerProperties.layerName) == 0) {

                layerFound = true;
                break;
                    
            }
            
        }
        
        if (!layerFound) {
            
            ret = false;
            
        }
        
    }
  
    return ret;
}

std::vector<const char*> HelloVulkan::GetRequiredExtensions()
{
    uint32_t sdlExtensionCount = 0;

    if (!SDL_Vulkan_GetInstanceExtensions(window, &sdlExtensionCount, nullptr)) {

        std::cout << "Could not get the number of required instance extensions from SDL." << std::endl;

        return {}; // Returns an empty vector

    }

    std::vector<const char*> extensions(sdlExtensionCount);

    if (!SDL_Vulkan_GetInstanceExtensions(window, &sdlExtensionCount, extensions.data())) {

        std::cout << "Could not get the names of required instance extensions from SDL." << std::endl;
        
        return {}; // Returns an empty vector

    }

    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

    if (enableValidationLayers) {

        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    }

    return extensions;
}

void HelloVulkan::ShowSupportedExtensions()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> supportedExtensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, supportedExtensions.data());

    std::cout << "Available Extensions:\n";

    for (const auto& extension : supportedExtensions) {

        std::cout << '\t' << extension.extensionName << '\n';

    }

}

VKAPI_ATTR VkBool32 VKAPI_CALL HelloVulkan::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    std::cout << "Validation layer: " << pCallbackData->pMessage << std::endl;

    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        
        // Message is important enough to show.
            
    }

    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    PFN_vkCreateDebugUtilsMessengerEXT createDUMEXT =
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

    if (createDUMEXT != nullptr) {

        return createDUMEXT(instance, pCreateInfo, pAllocator, pDebugMessenger);

    }
    else {

        return VK_ERROR_EXTENSION_NOT_PRESENT;

    }

}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    PFN_vkDestroyDebugUtilsMessengerEXT destroyDUMEXT =
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

    if (destroyDUMEXT != nullptr) {

        destroyDUMEXT(instance, debugMessenger, pAllocator);

    }

}

void HelloVulkan::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};

    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    createInfo.pfnUserCallback = DebugCallback;
    createInfo.pUserData = nullptr;
}

void HelloVulkan::Update()
{
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
}

void HelloVulkan::CleanUp()
{
    vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);

    vkDestroyDevice(logicalDevice, nullptr);

    if (enableValidationLayers) {

        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

    }

    // Destroy the Vulkan Surface
    vkDestroySurfaceKHR(instance, surface, nullptr);

    // Destroy the Vulkan Instance
    vkDestroyInstance(instance, nullptr);

    // Destroy the Window and Quit SDL
    SDL_DestroyWindow(window);
    SDL_Quit();
}