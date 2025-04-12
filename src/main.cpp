#define GLM_ENABLE_EXPERIMENTAL
#include <Jolt/Jolt.h>
#include <bgfx/bgfx.h>
#include "bx/debug.h"
#include <functional>
#include <memory>
#include <glm/glm.hpp>
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
#include "SceneManager.hpp"

void KeyEvent(Keycode key, KeyState state,
              std::unordered_map<uint64_t, Entity*>& entities) {
    bx::debugPrintf("Key event: %d, %d\n", key, state);
    if (state == KeyState::Release && key == Keycode::SPACE) {
        // entities[0].SetPhysicsPosition(glm::vec3{1.0f, 4.1f, 0.0f});
        // entities[1].SetPhysicsPosition(glm::vec3{0.0f, 2.0f, 0.0f});
        // entities[3].SetPhysicsPosition(glm::vec3{0.0f, 7.0f, 0.0f});
    } else if (state == KeyState::Release && key == Keycode::W) {
        for (auto& entity : entities) {
            if (entity.second->GetBodyType() == RigidBodyType::Dynamic) {
                entity.second->AddImpulse(glm::vec3{0.0f, 10.0f, 0.0f});
            }
        }
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

    Renderer renderer = Renderer("Hello World", 1280/10, 720/10);
    renderer.Init();

    Camera camera(renderer, glm::vec3(-6.0f, 3.0f, -6.0f),
                  glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                  60.0f, 1.0f, 100.0f);
    camera.LookAt({0.0f, 0.0f, 0.0f});

    uint32_t frame = 0;
    SceneManager::Initialize();
    renderer.SetViewClear();
    {

        {
            auto& scene = SceneManager::GetInstance();
            auto textureRef = scene.addTexture(
                Texture("assets/amongus.jpg", bgfx::TextureFormat::RGB8));
            auto meshRef =
                scene.addMeshContainer(MeshContainer("assets/Suzane.obj"));
            auto mesh2Ref =
                scene.addMeshContainer(MeshContainer("assets/Holder.obj"));
            auto colliderRef = scene.addCollider(
                Collider(ColliderType::Box, glm::vec3(0.0f), glm::vec3(0.0f),
                         glm::vec3(1.0f, 1.0f, 1.2f)));
            auto collider2Ref = scene.addCollider(Collider(
                ColliderType::Mesh, glm::vec3(0.0f), glm::vec3(0.0f),
                glm::vec3(1.0f, 1.0f, 1.0f), mesh2Ref.data->GetVertices(),
                mesh2Ref.data->GetIndices()));

            scene.addEntity(
                Primitive(PrimitiveType::Cube, RigidBodyType::Dynamic,
                          physicsCore, renderer.GetVertexLayout(),
                          *textureRef.data, glm::vec3{0.7f, 2.1f, 0.0f}));
            scene.addEntity(
                Primitive(PrimitiveType::Sphere, RigidBodyType::Dynamic,
                          physicsCore, renderer.GetVertexLayout(),
                          *textureRef.data, glm::vec3{0.0f, 0.0f, 0.0f}));
            scene.addEntity(Primitive(
                PrimitiveType::Plane, RigidBodyType::Static, physicsCore,
                renderer.GetVertexLayout(), *textureRef.data,
                glm::vec3{0.0f, -2.5f, 0.0f}, glm::vec3{0.0f, 0.0f, 0.0f},
                glm::vec3{10.0f, 1.0f, 10.0f}));
            scene.addEntity(MeshEntity(
                *meshRef.data, *colliderRef.data, RigidBodyType::Dynamic,
                physicsCore, renderer.GetVertexLayout(), *textureRef.data,
                glm::vec3{0.0f, 7.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 0.0f},
                glm::vec3{1.0f, 1.0f, 1.0f}));
            scene.addEntity(MeshEntity(
                *mesh2Ref.data, *collider2Ref.data, RigidBodyType::Static,
                physicsCore, renderer.GetVertexLayout(), *textureRef.data,
                glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 0.0f},
                glm::vec3{1.0f, 1.0f, 1.0f}));

            core.SetKeyEventCallback(std::bind(KeyEvent, std::placeholders::_1,
                                               std::placeholders::_2,
                                               scene.getEntities()));
        }

        auto& scene = SceneManager::GetInstance();
        bx::debugPrintf("Main loop started\n");
        while (!core.IsQuit()) {
            core.EventLoop();
            core.CallKeyboardEvent();

            renderer.UpdateWindowSize();

            accumulator += core.GetDeltaTime();

            while (accumulator >= FIXED_TIMESTEP) {
                physicsCore.Update(FIXED_TIMESTEP);
                for (auto& entity : scene.getEntities()) {
                    if (entity.second->GetBodyType() == RigidBodyType::Static) {
                        continue;
                    }
                    // Update the position of the primitive based on the physics
                    // simulation
                    auto transform =
                        physicsCore.GetBodyInterface().GetWorldTransform(
                            entity.second->GetBodyID());
                    entity.second->SetTransform(ToGLM(transform));
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

            for (auto& entity : scene.getEntities()) {
                entity.second->SetVertexBuffer();
                entity.second->SetIndexBuffer();
                entity.second->ApplyTransform();
                renderer.SetTextureUniform(entity.second->SetTexture());
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

    SceneManager::Shutdown();
    physicsCore.Shutdown();
    renderer.Shutdown();
    core.Shutdown();
    return 0;
}
