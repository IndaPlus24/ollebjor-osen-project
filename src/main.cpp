
#include <Jolt/Jolt.h>
#include <bgfx/bgfx.h>
#include "bx/debug.h"
#include "bx/math.h"
#include <functional>
#include <glm/glm.hpp>
#include <vector>
#include "Enums.hpp"

#include "Core.hpp"
#include "Renderer.hpp"
#include "Primitive.hpp"
#include "LuaCore.hpp"
#include "PhysicsCore.hpp"
#include "utils.hpp"

void KeyEvent(Keycode key, KeyState state, std::vector<Primitive>& primitives) {
    bx::debugPrintf("Key event: %d, %d\n", key, state);
    if (state == KeyState::Release && key == Keycode::SPACE) {
        primitives[0].SetPhysicsPosition(glm::vec3{1.0f, 4.1f, 0.0f});
        primitives[1].SetPhysicsPosition(glm::vec3{0.0f, 2.0f, 0.0f});
    } else if (state == KeyState::Release && key == Keycode::W) {
        primitives[1].AddImpulse({1.0f, 10.0f, 0.0f});
        bx::debugPrintf("Added impules\n");
    }
}

int main(int argc, char** argv) {

    const double FIXED_TIMESTEP = 1.0f / 60.0f;
    double accumulator = 0;
    bx::debugPrintf("Starting application\n");

    LuaCore lua;
    lua.Init();
    lua.Run("scripts/test.lua");

    PhysicsCore physicsCore = PhysicsCore();
    physicsCore.Initialize();

    Core core = Core();
    core.Init();

    Renderer renderer = Renderer("Hello World", 1280, 720);
    renderer.Init();


    renderer.SetViewClear();
    {
        Texture texture = Texture("assets/amongus.jpg", bgfx::TextureFormat::RGB8);

        std::vector<Primitive> primitives;
        primitives.emplace_back(PrimitiveType::Cube, RigidBodyType::Dynamic,
                                physicsCore, renderer.GetVertexLayout(),
                                texture, glm::vec3{0.7f, 2.1f, 0.0f});
        primitives.emplace_back(PrimitiveType::Sphere, RigidBodyType::Dynamic,
                                physicsCore, renderer.GetVertexLayout(),
                                texture, glm::vec3{0.0f, 0.0f, 0.0f});
        primitives.emplace_back(
            PrimitiveType::Plane, RigidBodyType::Static, physicsCore,
            renderer.GetVertexLayout(), texture, glm::vec3{0.0f, -2.5f, 0.0f},
            glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{10.0f, 1.0f, 10.0f});
        core.SetKeyEventCallback(std::bind(KeyEvent, std::placeholders::_1,
                                           std::placeholders::_2,
                                           std::ref(primitives)));

        bx::debugPrintf("Main loop started\n");
        while (!core.IsQuit()) {
            core.EventLoop();
            core.CallKeyboardEvent();

            renderer.UpdateWindowSize();

            accumulator += core.GetDeltaTime();

            while (accumulator >= FIXED_TIMESTEP) {
                physicsCore.Update(FIXED_TIMESTEP);
                for (auto& primitive : primitives) {
                    if (primitive.GetBodyType() == RigidBodyType::Static) {
                        continue;
                    }
                    // Update the position of the primitive based on the physics
                    // simulation
                    auto transform =
                        physicsCore.GetBodyInterface().GetWorldTransform(
                            primitive.GetBodyID());
                    primitive.SetTransform(ToGLM(transform));
                }
                accumulator -= FIXED_TIMESTEP;
            }

            // This dummy draw call is here to make sure that view 0 is cleared
            // if no other draw calls are submitted to view 0.
            bgfx::touch(0);

            uint32_t width, height;
            renderer.GetWindowSize(width, height);

            const bx::Vec3 at = {0.0f, 0.0f, 0.0f};
            const bx::Vec3 eye = {-6.0f, 3.0f, -6.0f};
            float view[16];
            bx::mtxLookAt(view, eye, at);
            float proj[16];
            bx::mtxProj(proj, 60.0f, float(width) / float(height), 0.1f, 100.0f,
                        bgfx::getCaps()->homogeneousDepth);
            bgfx::setViewTransform(0, view, proj);

            for (auto& primitive : primitives) {
                primitive.SetVertexBuffer();
                primitive.SetIndexBuffer();
                primitive.ApplyTransform();
                renderer.SetTextureUniform(primitive.SetTexture());
                bgfx::submit(0, renderer.GetProgramHandle());
            }

            // Advance to next frame. Process submitted rendering primitives.
            bgfx::frame();
        }
    }

    physicsCore.Shutdown();
    renderer.Shutdown();
    core.Shutdown();
    return 0;
}
