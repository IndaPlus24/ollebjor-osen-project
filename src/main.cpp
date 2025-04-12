#define GLM_ENABLE_EXPERIMENTAL
#include <Jolt/Jolt.h>
#include <bgfx/bgfx.h>
#include "bx/debug.h"
#include <functional>
#include <glm/glm.hpp>
#include <vector>
#include "Enums.hpp"

#include "Entity.hpp"
#include "Core.hpp"
#include "Renderer.hpp"
#include "Primitive.hpp"
#include "Texture.hpp"
#include "LuaCore.hpp"
#include "PhysicsCore.hpp"
#include "utils.hpp"
#include "MeshContainer.hpp"
#include "MeshEntity.hpp"
#include "Collider.hpp"
#include "Camera.hpp"

void KeyEvent(Keycode key, KeyState state, std::vector<Entity>& primitives) {
    bx::debugPrintf("Key event: %d, %d\n", key, state);
    if (state == KeyState::Release && key == Keycode::SPACE) {
        primitives[0].SetPhysicsPosition(glm::vec3{1.0f, 4.1f, 0.0f});
        primitives[1].SetPhysicsPosition(glm::vec3{0.0f, 2.0f, 0.0f});
        primitives[3].SetPhysicsPosition(glm::vec3{0.0f, 7.0f, 0.0f});
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

    Camera camera(renderer, glm::vec3(-6.0f, 3.0f, -6.0f),
                  glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                  60.0f, 1.0f, 100.0f);
    camera.LookAt({0.0f, 0.0f, 0.0f});

    uint32_t frame = 0;
    renderer.SetViewClear();
    {
        Texture texture =
            Texture("assets/amongus.jpg", bgfx::TextureFormat::RGB8);
        MeshContainer mesh("assets/Suzane.obj");
        MeshContainer mesh2("assets/Holder.obj");
        Collider collider(ColliderType::Box, glm::vec3(0.0f), glm::vec3(0.0f),
                          glm::vec3(1.0f, 1.0f, 1.2f));
        Collider collider2(ColliderType::Mesh, glm::vec3(0.0f), glm::vec3(0.0f),
                           glm::vec3(1.0f, 1.0f, 1.0f), mesh2.GetVertices(),
                           mesh2.GetIndices());

        std::vector<Entity> primitives;
        primitives.reserve(5);
        primitives.push_back(Primitive(
            PrimitiveType::Cube, RigidBodyType::Dynamic, physicsCore,
            renderer.GetVertexLayout(), texture, glm::vec3{0.7f, 2.1f, 0.0f}));
        primitives.push_back(Primitive(
            PrimitiveType::Sphere, RigidBodyType::Dynamic, physicsCore,
            renderer.GetVertexLayout(), texture, glm::vec3{0.0f, 0.0f, 0.0f}));
        primitives.push_back(Primitive(
            PrimitiveType::Plane, RigidBodyType::Static, physicsCore,
            renderer.GetVertexLayout(), texture, glm::vec3{0.0f, -2.5f, 0.0f},
            glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{10.0f, 1.0f, 10.0f}));
        primitives.push_back(MeshEntity(
            mesh, collider, RigidBodyType::Dynamic, physicsCore,
            renderer.GetVertexLayout(), texture, glm::vec3{0.0f, 7.0f, 0.0f},
            glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{1.0f, 1.0f, 1.0f}));
        primitives.push_back(MeshEntity(
            mesh2, collider2, RigidBodyType::Static, physicsCore,
            renderer.GetVertexLayout(), texture, glm::vec3{0.0f, 0.0f, 0.0f},
            glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{1.0f, 1.0f, 1.0f}));

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
                core.CallPhysicsStep(FIXED_TIMESTEP);
                accumulator -= FIXED_TIMESTEP;
            }
            core.CallUpdate(core.GetDeltaTime());

            // This dummy draw call is here to make sure that view 0 is cleared
            // if no other draw calls are submitted to view 0.
            bgfx::touch(0);

            // Update the camera position to follow a cricle around {0, 0, 0}
            camera.SetPosition(glm::vec3(10.0f * cos(frame * 0.01f), 5.0f,
                                         10.0f * sin(frame * 0.01f)));

            camera.SetProjection();
            camera.SetViewTransform(0);

            for (auto& primitive : primitives) {
                primitive.SetVertexBuffer();
                primitive.SetIndexBuffer();
                primitive.ApplyTransform();
                renderer.SetTextureUniform(primitive.SetTexture());
                bgfx::submit(0, renderer.GetProgramHandle());
            }

            // Advance to next frame. Process submitted rendering primitives.
            bgfx::frame();
            frame++;
            if (frame > 628) {
                frame = 0;
            }
        }
    }

    physicsCore.Shutdown();
    renderer.Shutdown();
    core.Shutdown();
    return 0;
}
