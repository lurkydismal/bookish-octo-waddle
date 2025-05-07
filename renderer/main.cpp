#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>  // for glfwGetX11Window, glfwGetX11Display
#include <vulkan/vulkan.h>

#undef Success
#undef Error

#include <filament/Renderer.h>
#include <filament/Camera.h>
#include <filament/Engine.h>
#include <filament/IndexBuffer.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>
#include <filament/RenderableManager.h>
#include <filament/Scene.h>
#include <filament/Skybox.h>
#include <filament/TransformManager.h>
#include <filament/VertexBuffer.h>
#include <filament/View.h>

#include <utils/EntityManager.h>

#include <cmath>
#include <iostream>
#include <stdlib.h>

#include "bc.h"

using namespace filament;
using namespace filament::math;
using namespace utils;

struct Vertex {
    filament::math::float2 position;
    uint32_t color;
};

static const Vertex TRIANGLE_VERTICES[3] = {
    {{1, 0}, 0xffff0000u},
    {{cos(M_PI * 2 / 3), sin(M_PI * 2 / 3)}, 0xff00ff00u},
    {{cos(M_PI * 4 / 3), sin(M_PI * 4 / 3)}, 0xff0000ffu},
};
static constexpr uint16_t TRIANGLE_INDICES[3] = { 0, 1, 2 };
extern const uint8_t RESOURCES_BAKEDCOLOR_DATA[];

const size_t RESOURCES_BAKEDCOLOR_SIZE = sizeof(RESOURCES_BAKEDCOLOR_DATA);

bool pickPhysicalDeviceAndCreateLogical(VkInstance instance,
                                        VkSurfaceKHR surface,
                                        VkPhysicalDevice* outPhysicalDevice,
                                        uint32_t*  outQueueFamilyIndex,
                                        VkDevice*   outDevice,
                                        VkQueue*    outQueue) {
    // 1) Enumerate physical devices
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
    if (deviceCount == 0) {
        fprintf(stderr, "No Vulkan GPUs found\n");
        return false;
    }
    VkPhysicalDevice* devices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

    // 2) For each device, look for a queue family that supports GRAPHICS + PRESENT
    for (uint32_t d = 0; d < deviceCount; d++) {
        VkPhysicalDevice phys = devices[d];

        // Query queue families
        uint32_t queueCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(phys, &queueCount, NULL);
        VkQueueFamilyProperties* families = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * queueCount);
        vkGetPhysicalDeviceQueueFamilyProperties(phys, &queueCount, families);

        for (uint32_t i = 0; i < queueCount; i++) {
            // Check graphics support
            bool supportsGraphics = families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT;

            // Check present support for our surface
            VkBool32 supportsPresent = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(phys, i, surface, &supportsPresent);

            if (supportsGraphics && supportsPresent) {
                // Found a good family!
                *outPhysicalDevice   = phys;
                *outQueueFamilyIndex = i;

                // 3) Create the logical device with the VK_KHR_swapchain extension
                float priority = 1.0f;
                VkDeviceQueueCreateInfo queueInfo = {
                    .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                    .queueFamilyIndex = i,
                    .queueCount       = 1,
                    .pQueuePriorities = &priority,
                };
                const char* deviceExts[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
                VkDeviceCreateInfo deviceInfo = {
                    .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                    .queueCreateInfoCount    = 1,
                    .pQueueCreateInfos       = &queueInfo,
                    .enabledExtensionCount   = 1,
                    .ppEnabledExtensionNames = deviceExts,
                    // you can also enable features here if needed
                };

                if (vkCreateDevice(phys, &deviceInfo, NULL, outDevice) != VK_SUCCESS) {
                    fprintf(stderr, "Failed to create logical device\n");
                    free(families);
                    continue;
                }

                // 4) Retrieve the VkQueue handle
                vkGetDeviceQueue(*outDevice, i, 0, outQueue);

                free(families);
                free(devices);
                return true;
            }
        }
        free(families);
    }

    free(devices);
    fprintf(stderr, "No suitable GPU + queue family found\n");
    return false;
}

int main() {
    // 1) Init GLFW for EGL + OpenGL
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // No OpenGL context
    GLFWwindow* window = glfwCreateWindow(640, 480, "Vulkan Window", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // 3) Grab the native X11 Window handle
    void* nativeWindow = (void*)(uintptr_t)glfwGetX11Window(window);

    // 4) Create Filament Engine / SwapChain / Renderer
    using namespace filament;
    using namespace utils;

    VkInstance       instance = /* your vkInstance */;
    VkSurfaceKHR     surface  = /* created via glfwCreateWindowSurface() */;
    VkPhysicalDevice gpu;
    uint32_t         queueFamily;
    VkDevice         device;
    VkQueue          graphicsQueue;

    if (!pickPhysicalDeviceAndCreateLogical(instance,
                                            surface,
                                            &gpu,
                                            &queueFamily,
                                            &device,
                                            &graphicsQueue)) {
        return -1;
    }

    // 2) Vulkan instance + surface
    uint32_t extCount = 0;
    const char** exts = glfwGetRequiredInstanceExtensions(&extCount);
    VkInstanceCreateInfo instInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    instInfo.enabledExtensionCount   = extCount;
    instInfo.ppEnabledExtensionNames = exts;
    vkCreateInstance(&instInfo, nullptr, &vkInstance);

    VkSurfaceKHR surface;
    glfwCreateWindowSurface(vkInstance, nativeWindow, nullptr, &surface);

    // 3) Pick GPU & create VkDevice + Queue
    //    (Select a physicalDevice with a graphics+present queue,
    //     create a VkDevice, vkGetDeviceQueue(...,&graphicsQueue)

    // 4) Tell Filament about Vulkan
    filament::VulkanContext ctx {
        .instance         = instance,
        .physicalDevice   = gpu,
        .device           = device,
        .queue            = graphicsQueue,
        .queueFamilyIndex = queueFamily
    };

    Engine*   engine    = Engine::Builder().
        .backend(Engine::Backend::VULKAN)
        .vulkanContext(&vkCtx)
        .build();
    SwapChain*swapChain = engine->createSwapChain((void*)surface);
    Renderer* renderer  = engine->createRenderer();

    // 5) Build View / Scene / Camera
    Camera* camera = engine->createCamera(EntityManager::get().create());
    View*   view   = engine->createView();
    Scene*  scene  = engine->createScene();
    view->setCamera(camera);
    view->setScene(scene);

    VertexBuffer* vertexBuffer = VertexBuffer::Builder()
        .vertexCount(3)
        .bufferCount(1)
        .attribute(VertexAttribute::POSITION, 0, VertexBuffer::AttributeType::FLOAT2, 0, 12)
        .attribute(VertexAttribute::COLOR,    0, VertexBuffer::AttributeType::UBYTE4, 8, 12)
        .normalized(VertexAttribute::COLOR)
        .build(*engine);
    vertexBuffer->setBufferAt(*engine, 0,
            VertexBuffer::BufferDescriptor(TRIANGLE_VERTICES, 3 * (2+4) * sizeof(float), nullptr));

    IndexBuffer* indexBuffer = IndexBuffer::Builder()
        .indexCount(3)
        .bufferType(IndexBuffer::IndexType::USHORT)
        .build(*engine);
    indexBuffer->setBuffer(*engine,
        IndexBuffer::BufferDescriptor(TRIANGLE_INDICES, 3 * sizeof(uint16_t), nullptr));

    // The material instance is obtained from a material, itself loaded from a binary blob generated by matc:
    Material* material = Material::Builder()
        .package((void*) RESOURCES_BAKEDCOLOR_DATA, RESOURCES_BAKEDCOLOR_SIZE)
        .build(*engine);
    MaterialInstance* materialInstance = material->createInstance();

    // 7) Create renderable and add to scene
    Entity renderable = EntityManager::get().create();
    RenderableManager::Builder(1)
        .boundingBox({{ -1, -1, -1 },{ 1, 1, 1 }})
        .material(0, materialInstance)
        .geometry(0, RenderableManager::PrimitiveType::TRIANGLES, vertexBuffer, indexBuffer, 0, 3)
        .culling(false)
        .build(*engine, renderable);
    scene->addEntity(renderable);

    // 8) Render loop
    while (!glfwWindowShouldClose(window)) {
        if (renderer->beginFrame(swapChain)) {
            renderer->render(view);
            renderer->endFrame();
        }
        glfwPollEvents();
    }

    // 9) Cleanup
    Engine::destroy(&engine);
    glfwDestroyWindow(window);
    glfwTerminate();

    printf("end\n");

    return 0;
}
