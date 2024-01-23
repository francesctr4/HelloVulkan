#include "HelloVulkan.h"

#include "Globals.h"

static const std::vector<Vertex> vertices = {

    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}

};

static const std::vector<uint16_t> indices = {

    0, 1, 2, 
    2, 3, 0

};

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

    Uint32 flags = SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

    window = SDL_CreateWindow(gAppName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, gWindowWidth, gWindowHeight, flags);

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
    CreateRenderPass();
    CreateDescriptorSetLayout();
    CreateGraphicsPipeline(); 
    CreateFramebuffers();
    CreateCommandPool();
    CreateVertexBuffer();
    CreateIndexBuffer();
    CreateUniformBuffers();
    CreateDescriptorPool();
    CreateDescriptorSets();
    CreateCommandBuffers();
    CreateSyncObjects();
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

    for (size_t i = 0; i < swapChainImages.size(); i++) {

        VkImageViewCreateInfo createInfo{};

        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];

        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(logicalDevice, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
            
            std::cout << "Failed to create image views!" << std::endl;
            
        }
        else {

            std::cout << "Image view " << ("%d", i + 1) << " created successfully." << std::endl;

        }

    }

}

void HelloVulkan::CreateRenderPass()
{
    VkAttachmentDescription colorAttachment{};

    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};

    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};

    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};

    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;

    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;

    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};

    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;

    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;
   
    if (vkCreateRenderPass(logicalDevice, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        
        std::cout << "Failed to create render pass!" << std::endl;
        
    }
    else {

        std::cout << "Render pass created succesfully." << std::endl;

    }

}

void HelloVulkan::CreateDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding uboLayoutBinding{};

    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;

    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;
    
    if (vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {

         std::cout << "Failed to create Descriptor Set Layout!" << std::endl;
            
    }
    else {

        std::cout << "Descriptor Set Layout created successfully." << std::endl;

    }

}

void HelloVulkan::CreateGraphicsPipeline()
{
    auto vertShaderCode = ReadFile("Shaders/vert.spv");
    auto fragShaderCode = ReadFile("Shaders/frag.spv");
    
    VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

    // Vertex Shader

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};

    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;

    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    vertShaderStageInfo.pSpecializationInfo = nullptr;

    // Fragment Shader

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};

    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;

    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    fragShaderStageInfo.pSpecializationInfo = nullptr;

    // Shader Stages Creation

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    // Vertex input

    VkPipelineVertexInputStateCreateInfo vertexInput{};

    vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    vertexInput.vertexBindingDescriptionCount = 1;
    vertexInput.pVertexBindingDescriptions = &bindingDescription;

    vertexInput.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInput.pVertexAttributeDescriptions = attributeDescriptions.data();

    // Input Assembly

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};

    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // Viewports and scissors

    /*VkViewport viewport{};

    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChainExtent.width;
    viewport.height = (float)swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;*/

    /*VkRect2D scissor{};

    scissor.offset = { 0, 0 };
    scissor.extent = swapChainExtent;*/

    VkPipelineViewportStateCreateInfo viewportState{};

    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

    viewportState.viewportCount = 1;
    //viewportState.pViewports = &viewport;

    viewportState.scissorCount = 1;
    //viewportState.pScissors = &scissor;

    // Rasterizer

    VkPipelineRasterizationStateCreateInfo rasterizer{};

    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;

    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;

    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; 
    rasterizer.depthBiasClamp = 0.0f; 
    rasterizer.depthBiasSlopeFactor = 0.0f; 

    // Multisampling

    VkPipelineMultisampleStateCreateInfo multisampling{};

    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;

    multisampling.alphaToCoverageEnable = VK_FALSE; 
    multisampling.alphaToOneEnable = VK_FALSE; 

    // Depth and Stencil Testing

    //VkPipelineDepthStencilStateCreateInfo depthStencil{};
    // [...]

    // Color Blending

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};

    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | 
        VK_COLOR_COMPONENT_G_BIT | 
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;

    colorBlendAttachment.blendEnable = VK_FALSE;

    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; 
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;

    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; 
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; 
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; 

    //colorBlendAttachment.blendEnable = VK_TRUE;

    //colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    //colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    //colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;

    //colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    //colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    //colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};

    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;

    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    // Dynamic State

    std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

    VkPipelineDynamicStateCreateInfo dynamicState{};

    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    // Pipeline Layout

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    pipelineLayoutInfo.setLayoutCount = 1; 
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout; 

    pipelineLayoutInfo.pushConstantRangeCount = 0; 
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    
    if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {

        std::cout << "Failed to create pipeline layout!" << std::endl;
        
    }
    else {

        std::cout << "Pipeline layout created successfully." << std::endl;

    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};

    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;

    pipelineInfo.pVertexInputState = &vertexInput;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;

    pipelineInfo.layout = pipelineLayout;

    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; 
    pipelineInfo.basePipelineIndex = -1; 

    if (vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {

        std::cout << "Failed to create Graphics Pipeline!" << std::endl;
       
    }
    else {

        std::cout << "Graphics Pipeline created successfully." << std::endl;

    }

    vkDestroyShaderModule(logicalDevice, fragShaderModule, nullptr);
    vkDestroyShaderModule(logicalDevice, vertShaderModule, nullptr);
}

void HelloVulkan::CreateFramebuffers()
{
    swapChainFramebuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        
        VkImageView attachments[] = { swapChainImageViews[i] };
       
        VkFramebufferCreateInfo framebufferInfo{};

        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

        framebufferInfo.renderPass = renderPass;

        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;

        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;

        framebufferInfo.layers = 1;
        
        if (vkCreateFramebuffer(logicalDevice, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {

            std::cout << "Failed to create framebuffer!" << std::endl;
            
        }
        else {

            std::cout << "Framebuffer " << ("%d", i + 1) << " created successfully." << std::endl;

        }
        
    }

}

void HelloVulkan::CreateCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(physicalDevice);
    
    VkCommandPoolCreateInfo poolInfo{};

    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {

        std::cout << "Failed to create command pool!" << std::endl;
        
    }
    else {

        std::cout << "Command pool created succesfully." << std::endl;

    }

}

void HelloVulkan::CreateVertexBuffer()
{
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    CreateBuffer(bufferSize, 
                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                 stagingBuffer,
                 stagingBufferMemory);

    void* VBO;
    vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &VBO);
    memcpy(VBO, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(logicalDevice, stagingBufferMemory);
    
    CreateBuffer(bufferSize, 
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                 vertexBuffer,
                 vertexBufferMemory);

    CopyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
    
}

void HelloVulkan::CreateIndexBuffer()
{
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    CreateBuffer(bufferSize,
                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer,
                 stagingBufferMemory);

    void* EBO;
    vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &EBO);
    memcpy(EBO, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(logicalDevice, stagingBufferMemory);

    CreateBuffer(bufferSize,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 indexBuffer,
                 indexBufferMemory);

    CopyBuffer(stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
}

void HelloVulkan::CreateUniformBuffers()
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    
    uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);
    
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

        CreateBuffer(bufferSize, 
                     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                     uniformBuffers[i],
                     uniformBuffersMemory[i]);
        
        vkMapMemory(logicalDevice, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
        
    }

}

void HelloVulkan::CreateDescriptorPool()
{
    VkDescriptorPoolSize poolSize{};

    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    poolInfo.flags = 0;

    if (vkCreateDescriptorPool(logicalDevice, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {

        std::cout << "Failed to create Descriptor Pool!" << std::endl;
        
    }
    else {

        std::cout << "Descriptor Pool created successfully." << std::endl;

    }

}

void HelloVulkan::CreateDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

    if (vkAllocateDescriptorSets(logicalDevice, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {

        std::cout << "Failed to allocate Descriptor Sets!" << std::endl;
        
    }
    else {

        std::cout << "Descriptor Sets allocated successfully." << std::endl;

    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

        VkDescriptorBufferInfo bufferInfo{};

        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkWriteDescriptorSet descriptorWrite{};

        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;

        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;

        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = nullptr;
        descriptorWrite.pTexelBufferView = nullptr;

        vkUpdateDescriptorSets(logicalDevice, 1, &descriptorWrite, 0, nullptr);

    }

}

void HelloVulkan::CreateCommandBuffers()
{
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};

    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;

    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();
   
    if (vkAllocateCommandBuffers(logicalDevice, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {

        std::cout << "Failed to allocate command buffers!" << std::endl;
        
    }
    else {

        std::cout << "Command buffer allocated successfully." << std::endl;

    }

}

void HelloVulkan::CreateSyncObjects()
{
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

        if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(logicalDevice, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

            std::cout << "Failed to create synchronization objects for a frame!" << std::endl;

        }

    }

}

void HelloVulkan::RecreateSwapChain()
{
    int width = 0, height = 0;
    SDL_Vulkan_GetDrawableSize(window, &width, &height);

    while (width == 0 || height == 0) {

        SDL_Vulkan_GetDrawableSize(window, &width, &height);
        SDL_WaitEvent(nullptr);

    }

    vkDeviceWaitIdle(logicalDevice);
    
    CleanUpSwapChain();

    CreateSwapChain();
    CreateImageViews();
    CreateFramebuffers();
}

void HelloVulkan::CleanUpSwapChain()
{
    for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {

        vkDestroyFramebuffer(logicalDevice, swapChainFramebuffers[i], nullptr);

    }

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {

        vkDestroyImageView(logicalDevice, swapChainImageViews[i], nullptr);

    }

    vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
}

void HelloVulkan::UpdateUniformBuffer(uint32_t currentImage)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject UBO{};
    UBO.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    UBO.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    UBO.projection = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 10.0f);
    UBO.projection[1][1] *= -1;

    memcpy(uniformBuffersMapped[currentImage], &UBO, sizeof(UBO));
}

void HelloVulkan::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
 
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;
    
    VkCommandBuffer commandBuffer;

    vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0; 
    copyRegion.size = size;

    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    
    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);

    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(logicalDevice, commandPool, 1, &commandBuffer);

}

void HelloVulkan::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferInfo.flags = 0;

    if (vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {

        std::cout << "Failed to create Buffer!" << std::endl;

    }
    else {

        std::cout << "Buffer created successfully." << std::endl;

    }

    VkMemoryRequirements memRequirements;

    vkGetBufferMemoryRequirements(logicalDevice, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {

        std::cout << "Failed to allocate Buffer Memory!" << std::endl;

    }
    else {

        std::cout << "Buffer Memory allocated successfully." << std::endl;

    }

    vkBindBufferMemory(logicalDevice, buffer, bufferMemory, 0);
    
}

uint32_t HelloVulkan::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;

    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {

        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {

            return i;
            
        }
        
    }
    
    throw std::runtime_error("Failed to find suitable memory type!");

}

void HelloVulkan::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};

    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; 
    beginInfo.pInheritanceInfo = nullptr; 
    
    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {

        std::cout << "Failed to begin recording command buffer!" << std::endl;
            
    }

    VkRenderPassBeginInfo renderPassInfo{};

    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];

    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChainExtent;

    VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    VkViewport viewport{};

    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChainExtent.width);
    viewport.height = static_cast<float>(swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    
    VkRect2D scissor{};

    scissor.offset = { 0, 0 };
    scissor.extent = swapChainExtent;

    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkBuffer vertexBuffers[] = { vertexBuffer };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);

    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {

        std::cout << "Failed to record command buffer!" << std::endl;
        
    }

}

std::vector<char> HelloVulkan::ReadFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {

        std::cout << "Failed to open file!" << std::endl;

    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

VkShaderModule HelloVulkan::CreateShaderModule(const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};

    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;

    if (vkCreateShaderModule(logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {

        std::cout << "Failed to create shader module!" << std::endl;

    }
    else {

        std::cout << "Shader module created successfully!" << std::endl;

    }

    return shaderModule;
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

void HelloVulkan::DrawFrame()
{
    vkWaitForFences(logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(logicalDevice, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
    
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {

        RecreateSwapChain();

        return;
        
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        
        std::cout << "Failed to acquire swap chain image!" << std::endl;
        
    }

    UpdateUniformBuffer(currentFrame);

    vkResetFences(logicalDevice, 1, &inFlightFences[currentFrame]);

    vkResetCommandBuffer(commandBuffers[currentFrame], 0);

    RecordCommandBuffer(commandBuffers[currentFrame], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    
    VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;

    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {

        std::cout << "Failed to submit draw command buffer!" << std::endl;
            
    }

    VkPresentInfoKHR presentInfo{};

    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { swapChain };

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    presentInfo.pResults = nullptr;

    result = vkQueuePresentKHR(presentQueue, &presentInfo);
   
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {

        framebufferResized = false;
        RecreateSwapChain();
        
    }
    else if (result != VK_SUCCESS) {

         std::cout << "Failed to present swap chain image!" << std::endl;
        
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

}

void HelloVulkan::Update()
{
    bool quit = false;
    SDL_Event e;

    while (!quit) {

        // Manage Window Events

        while (SDL_PollEvent(&e) != 0) {

            switch (e.type)
            {
                case SDL_QUIT:

                    quit = true;

                    break;

                case SDL_WINDOWEVENT:

                    if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) { // Window has been resized

                        framebufferResized = true;

                    }

                    break;
            }

        }

        // Main Loop

        DrawFrame();

    }

    vkDeviceWaitIdle(logicalDevice);
}

void HelloVulkan::CleanUp()
{
    CleanUpSwapChain();

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

        vkDestroyBuffer(logicalDevice, uniformBuffers[i], nullptr);
        vkFreeMemory(logicalDevice, uniformBuffersMemory[i], nullptr);
        
    }

    vkDestroyDescriptorPool(logicalDevice, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout, nullptr);

    vkDestroyBuffer(logicalDevice, indexBuffer, nullptr);
    vkFreeMemory(logicalDevice, indexBufferMemory, nullptr);

    vkDestroyBuffer(logicalDevice, vertexBuffer, nullptr);
    vkFreeMemory(logicalDevice, vertexBufferMemory, nullptr);

    vkDestroyPipeline(logicalDevice, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(logicalDevice, pipelineLayout, nullptr);

    vkDestroyRenderPass(logicalDevice, renderPass, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

        vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(logicalDevice, renderFinishedSemaphores[i], nullptr);
        vkDestroyFence(logicalDevice, inFlightFences[i], nullptr);

    }

    vkDestroyCommandPool(logicalDevice, commandPool, nullptr);

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