
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include "bx/math.h"
#include <Jolt/Jolt.h>
#include <functional>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include "Enums.hpp"
#include "Core.hpp"
#include "Renderer.hpp"
#include "Primitive.hpp"

void test(Keycode key, KeyState state) {
    if (state == KeyState::Pressed)
        std::cout << "Key pressed: " << (int)key << std::endl;
    else
        std::cout << "Key released: " << (int)key << std::endl;
}

void test2(int x, int y, int xrel, int yrel) {
    std::cout << "Mouse: x: " << x << " y: " << y << " xrel: " << xrel
              << " yrel: " << yrel << std::endl;
}

void test3(int x, int y, MouseButton button, KeyState state) {
    std::cout << "Mouse button: x: " << x << " y: " << y
              << " button: " << (int)button << " state: " << (int)state
              << std::endl;
}

void test4(int x, int y) {
    std::cout << "Mouse wheel: x: " << x << " y: " << y << std::endl;
}

int main(int argc, char** argv) {

    const double FIXED_TIMESTEP = 1.0f / 60.0f;
    uint64_t accumulator = 0;

    Core core = Core();
    core.Init();
    core.SetKeyEventCallback(
        std::bind(test, std::placeholders::_1, std::placeholders::_2));
    core.SetMouseMoveEventCallback(
        std::bind(test2, std::placeholders::_1, std::placeholders::_2,
                  std::placeholders::_3, std::placeholders::_4));
    core.SetMouseButtonEventCallback(
        std::bind(test3, std::placeholders::_1, std::placeholders::_2,
                  std::placeholders::_3, std::placeholders::_4));
    core.SetMouseWheelEventCallback(
        std::bind(test4, std::placeholders::_1, std::placeholders::_2));

    Renderer renderer = Renderer("Hello World", 1280, 720);
    renderer.Init();

    renderer.SetViewClear();
    uint32_t counter = 0;
    {
        std::vector<Primitive> primitives;
        primitives.push_back(Primitive(PrimitiveType::Cube,
                                       renderer.GetVertexLayout(), 0xff0000ff));

        bx::debugPrintf("Main loop started\n");
        while (!core.IsQuit()) {
            core.EventLoop();
            core.CallKeyboardEvent();

            renderer.UpdateWindowSize();

            accumulator += core.GetDeltaTime();

            while (accumulator >= FIXED_TIMESTEP) {
                // FIXED TIMESTEP UPDATE for physics here
                accumulator -= FIXED_TIMESTEP;
            }

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
                primitive.AddRotation({1.0f, 1.0f, 0.0f});
                primitive.ApplyTransform();
                if (counter == 30) {
                    primitive.SetColor(0xff00ff00);
                } else if (counter == 60) {
                    primitive.SetColor(0xff0000ff);
                    counter = 0;
                }

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
