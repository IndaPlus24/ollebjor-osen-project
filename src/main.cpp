
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include "bx/math.h"
#include <Jolt/Jolt.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include "Enums.hpp"
#include "Core.hpp"
#include "Renderer.hpp"
#include "Primitive.hpp"

int main(int argc, char** argv) {
    Core core = Core();
    core.Init();

    Renderer renderer = Renderer("Hello World", 1280, 720);
    renderer.Init();

    renderer.SetViewClear();
    {
        std::vector<Primitive> primitives;
        primitives.emplace_back(PrimitiveType::Cube,
                                renderer.GetVertexLayout());
        primitives.emplace_back(
            PrimitiveType::Sphere, renderer.GetVertexLayout(),
            glm::vec3(0.0f, 1.5f, 0.0f), glm::vec3(0.0f), glm::vec3(0.75f));
        primitives.push_back(Primitive(
            PrimitiveType::Plane, renderer.GetVertexLayout(),
            glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.0f), glm::vec3(5.0f)));

        std::cout << "Main loop started" << std::endl;
        uint32_t counter = 0;
        while (!core.IsQuit()) {
            core.EventLoop();

            renderer.UpdateWindowSize();

            // This dummy draw call is here to make sure that view 0 is cleared
            // if no other draw calls are submitted to view 0.
            bgfx::touch(0);

            uint32_t width, height;
            renderer.GetWindowSize(width, height);

            const bx::Vec3 at = {0.0f, 0.0f, 0.0f};
            const bx::Vec3 eye = {0.0f, 0.0f, -6.0f};
            float view[16];
            bx::mtxLookAt(view, eye, at);
            float proj[16];
            bx::mtxProj(proj, 60.0f, float(width) / float(height), 0.1f, 100.0f,
                        bgfx::getCaps()->homogeneousDepth);
            bgfx::setViewTransform(0, view, proj);

            for (auto& primitive : primitives) {
                primitive.SetVertexBuffer();
                primitive.SetIndexBuffer();
                primitive.SetRotation({90.0f, 0.0f, 0.0f});
                primitive.ApplyTransform();
                bgfx::submit(0, renderer.GetProgramHandle());
            }

            // Advance to next frame. Process submitted rendering primitives.
            bgfx::frame();
            counter++;
        }
    }

    renderer.Shutdown();
    core.Shutdown();
    return 0;
}
