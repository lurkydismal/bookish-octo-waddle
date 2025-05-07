#include "renderer.h"

#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>  // for glfwGetX11Window, glfwGetX11Display
#include <glad/gl.h>

#undef Success
#undef Error

#include <filament/Engine.h>
#include <filament/SwapChain.h>
#include <filament/Renderer.h>
#include <filament/Scene.h>
#include <filament/View.h>
#include <filament/Camera.h>
#include <filament/VertexBuffer.h>
#include <filament/IndexBuffer.h>
#include <filament/RenderableManager.h>
#include <filament/Skybox.h>
#include <filament/Material.h>
#include <utils/EntityManager.h>

using namespace filament;
using namespace filament::math;
using namespace utils;

struct Vertex {
    filament::math::float2 position;
    uint32_t color;
};

// Your triangle data, generated/resources
static const Vertex TRIANGLE_VERTICES[3] = {
    {{1, 0}, 0xffff0000u},
    {{cos(M_PI * 2 / 3), sin(M_PI * 2 / 3)}, 0xff00ff00u},
    {{cos(M_PI * 4 / 3), sin(M_PI * 4 / 3)}, 0xff0000ffu},
};
static constexpr uint16_t TRIANGLE_INDICES[3] = { 0, 1, 2 };
const uint8_t RESOURCES_BAKEDCOLOR_DATA[] = {
    0x42, 0x41, 0x4B, 0x45, /* … etc … */ 
};

const size_t RESOURCES_BAKEDCOLOR_SIZE = sizeof(RESOURCES_BAKEDCOLOR_DATA);

int main() {
    // 1) Init GLFW
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Hello Triangle", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    // 2) Init glad (must be after context)
    if (!gladLoadGL( glfwGetProcAddress )) {
        fprintf(stderr, "Failed to initialize glad\n");
        return -1;
    }

    // 3) Create Filament objects
    Engine*    engine   = Engine::create();
    // On Linux/X11 you must pass the X11 Display* and Window (as void*)
    void*      nativeWindow = (void*)glfwGetX11Window(window);
    SwapChain* sc       = engine->createSwapChain(nativeWindow);
    Renderer*  renderer = engine->createRenderer();
    Scene*     scene    = engine->createScene();
    View*      view     = engine->createView();

    // Camera
    auto       camEntity = EntityManager::get().create();
    Camera*    cam       = engine->createCamera(camEntity);
    view->setCamera(cam);
    view->setScene(scene);

    // Skybox & disable post-processing
    scene->setSkybox(Skybox::Builder()
        .color({0.1f, 0.125f, 0.25f, 1.0f})
        .build(*engine));
    view->setPostProcessingEnabled(false);

    // 4) Build triangle mesh + material + renderable
    VertexBuffer* vb = VertexBuffer::Builder()
        .vertexCount(3)
        .bufferCount(1)
        .attribute(VertexAttribute::POSITION, 0, VertexBuffer::AttributeType::FLOAT2, 0, 12)
        .attribute(VertexAttribute::COLOR,    0, VertexBuffer::AttributeType::UBYTE4, 8, 12)
        .normalized(VertexAttribute::COLOR)
        .build(*engine);
    vb->setBufferAt(*engine, 0,
        VertexBuffer::BufferDescriptor(TRIANGLE_VERTICES, 3 * (2+4) * sizeof(float), nullptr));

    IndexBuffer* ib = IndexBuffer::Builder()
        .indexCount(3)
        .bufferType(IndexBuffer::IndexType::USHORT)
        .build(*engine);
    ib->setBuffer(*engine,
        IndexBuffer::BufferDescriptor(TRIANGLE_INDICES, 3 * sizeof(uint16_t), nullptr));

    Material* mat = Material::Builder()
        .package(RESOURCES_BAKEDCOLOR_DATA, RESOURCES_BAKEDCOLOR_SIZE)
        .build(*engine);

    Entity renderable = EntityManager::get().create();
    RenderableManager::Builder(1)
        .boundingBox({{-1,-1,-1},{1,1,1}})
        .geometry(0, RenderableManager::PrimitiveType::TRIANGLES, vb, ib)
        .material(0, mat->getDefaultInstance())
        .build(*engine, renderable);
    scene->addEntity(renderable);

    // 5) Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // update camera projection (e.g. rotating ortho)
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        float aspect = float(w) / float(h), Z = 1.5f;
        cam->setProjection(Camera::Projection::ORTHO,
                           -aspect*Z, aspect*Z, -Z, Z, 0.1, 10);

        if (renderer->beginFrame(sc)) {
            renderer->render(view);
            renderer->endFrame();
        }
        glfwSwapBuffers(window);
    }

    // 6) Cleanup (reverse order)
    engine->destroy(mat);
    engine->destroy(vb);
    engine->destroy(ib);
    engine->destroy(view);
    engine->destroy(renderer);
    engine->destroy(scene);
    engine->destroy(sc);
    engine->destroyCameraComponent(camEntity);
    EntityManager::get().destroy(camEntity);
    engine->destroy(camEntity);
    Engine::destroy(&engine);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

