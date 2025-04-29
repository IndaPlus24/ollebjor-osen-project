#define GLM_ENABLE_EXPERIMENTAL
#include <Jolt/Jolt.h>
#include <bgfx/bgfx.h>
#include "bx/debug.h"
#include <functional>
#include <glm/glm.hpp>
#include "Enums.hpp"

#include "Entity.hpp"
#include "Core.hpp"
#include "Renderer.hpp"
#include "Primitive.hpp"
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

    PhysicsCore physicsCore = PhysicsCore();
    physicsCore.Init();

    Core core = Core();
    core.Init();

    Renderer renderer = Renderer("Hello World", 1280, 720);
    renderer.Init();
    core.SetRenderer(&renderer);

    SceneManager::Initialize(physicsCore, renderer.GetVertexLayout(), renderer);

    uint32_t frame = 0;
    bgfx::frame();
    {

        {
            auto& scene = SceneManager::Get();
            Camera camera(renderer, glm::vec3(-6.0f, 3.0f, -6.0f),
                          glm::vec3(0.0f, 1.0f, 0.0f), 60.0f, 1.0f, 100.0f);
            auto cam = scene.AddCamera(std::move(camera));
            scene.SetActiveCamera(cam.id);
            auto textureRef = scene.AddTexture("assets/wood_planks_diff_2k.jpg",
                                               bgfx::TextureFormat::RGB8);
            auto texture2Ref = scene.AddTexture(
                "assets/wood_planks_nor_dx_2k.jpg", bgfx::TextureFormat::RGBA8);
            auto meshRef =
                scene.AddMeshContainer(MeshContainer("assets/Suzane.obj"));
            auto mesh2Ref =
                scene.AddMeshContainer(MeshContainer("assets/Holder.obj"));
            auto colliderRef = scene.AddCollider(
                Collider(ColliderType::Box, glm::vec3(0.0f), glm::vec3(0.0f),
                         glm::vec3(1.0f, 1.0f, 1.2f)));
            auto collider2Ref = scene.AddCollider(Collider(
                ColliderType::Mesh, glm::vec3(0.0f), glm::vec3(0.0f),
                glm::vec3(1.0f, 1.0f, 1.0f), mesh2Ref.data->GetVertices(),
                mesh2Ref.data->GetIndices()));

            scene.AddEntity(PrimitiveType::Cube, RigidBodyType::Dynamic,
                            textureRef.id, glm::vec3{0.7f, 2.1f, 0.0f});
            scene.AddEntity(
                Primitive(PrimitiveType::Sphere, RigidBodyType::Dynamic,
                          physicsCore, renderer.GetVertexLayout(),
                          *textureRef.data, glm::vec3{0.0f, 0.0f, 0.0f}));
            scene.AddEntity(Primitive(
                PrimitiveType::Plane, RigidBodyType::Static, physicsCore,
                renderer.GetVertexLayout(), *textureRef.data,
                glm::vec3{0.0f, -2.5f, 0.0f}, glm::vec3{0.0f, 0.0f, 0.0f},
                glm::vec3{10.0f, 1.0f, 10.0f}));
            scene.AddEntity(MeshEntity(
                *meshRef.data, colliderRef.data, RigidBodyType::Dynamic,
                physicsCore, renderer.GetVertexLayout(), *textureRef.data,
                glm::vec3{0.0f, 7.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 0.0f},
                glm::vec3{1.0f, 1.0f, 1.0f}));
            scene.AddEntity(MeshEntity(
                *mesh2Ref.data, collider2Ref.data, RigidBodyType::Static,
                physicsCore, renderer.GetVertexLayout(), *textureRef.data,
                glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 0.0f},
                glm::vec3{1.0f, 1.0f, 1.0f}));

            core.SetKeyEventCallback(std::bind(KeyEvent, std::placeholders::_1,
                                               std::placeholders::_2,
                                               scene.GetEntities()));
            lua.Run("scripts/test.lua");
        }

        auto& scene = SceneManager::Get();
        bx::debugPrintf("Main loop started\n");
        while (!core.IsQuit()) {
            core.EventLoop();
            core.CallKeyboardEvent();

            accumulator += core.GetDeltaTime();

            while (accumulator >= FIXED_TIMESTEP) {
                physicsCore.Update(FIXED_TIMESTEP);
                for (auto& entity : scene.GetEntities()) {
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
            auto cam = scene.GetActiveCamera();
            cam.data->SetPosition(glm::vec3(10.0f * cos(frame * 0.003f), 3.3f + 2.0f *
                                                        sin(frame * 0.008f),
                                            10.0f * sin(frame * 0.003f)));
            cam.data->SetProjection();

            auto albedo = scene.GetTexture(0);
            auto normal = scene.GetTexture(1);

            for (auto& entity : scene.GetEntities()) {
                // Start a rendering pass for every entity
                renderer.BeginPass(0);
                cam.data->SetViewTransform(0);
                entity.second->SetVertexBuffer();
                entity.second->SetIndexBuffer();
                entity.second->ApplyTransform();
                renderer.SetTextureUniforms(albedo.data->GetTextureHandle(),
                                            normal.data->GetTextureHandle());
                renderer.EndPass();
            }

            // Start the lighting pass
            renderer.BeginPass(1);
            renderer.EndPass();

            // Start the combine pass
            renderer.BeginPass(2);
            renderer.EndPass();

            // Advance to next frame. Process submitted rendering primitives.
            frame = bgfx::frame();
        }
    }

    SceneManager::Shutdown();
    physicsCore.Shutdown();
    renderer.Shutdown();
    core.Shutdown();
    return 0;
}
