#include <iostream>
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
#include "SceneImporter.hpp"

void KeyEvent(Keycode key, KeyState state,
              std::unordered_map<uint64_t, Entity*>& entities) {
    bx::debugPrintf("Key event: %d, %d\n", key, state);
    if (state == KeyState::Release && key == Keycode::W) {
        for (auto& entity : entities) {
            if (entity.second->GetBodyType() == RigidBodyType::Dynamic) {
                entity.second->AddImpulse(glm::vec3{0.0f, 10.0f, 0.0f});
                bx::debugPrintf("Added impules\n");
            }
        }
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
    core.SetWindowMinimizedCallback(
        [&]() { lua.FireSignal(lua.WindowService.Minimized); });

    Renderer renderer = Renderer("Hello World", 1280, 720);
    renderer.Init();
    core.SetRenderer(&renderer);
    SceneImporter sceneImporter;

    SceneManager::Initialize(physicsCore, renderer.GetVertexLayout(), renderer,
                             sceneImporter);

    uint32_t frame = 0;
    bgfx::frame();
    {

        {
            auto& scene = SceneManager::Get();
            Camera camera(renderer, glm::vec3(3.0f, 2.0f, 0.0f),
                          glm::vec3(0.0f, 1.0f, 0.0f), 60.0f, 1.0f, 100.0f);
            camera.LookAt(glm::vec3(0.0f, 0.5f, 0.0f));
            auto cam = scene.AddCamera(std::move(camera));
            scene.SetActiveCamera(cam.id);
            auto materialRef =
                scene.AddMaterial("assets/wood_planks_diff_2k.jpg",
                                  "assets/wood_planks_nor_dx_2k.jpg");
            scene.AddEntity(PrimitiveType::Cube, RigidBodyType::Dynamic,
                            materialRef.id, glm::vec3{0.7f, 7.1f, 0.0f});
            scene.AddEntity(
                Primitive(PrimitiveType::Sphere, RigidBodyType::Dynamic,
                          physicsCore, renderer.GetVertexLayout(),
                          materialRef.id, glm::vec3{0.0f, 5.0f, 2.0f}));
            scene.AddEntity(PrimitiveType::Plane, RigidBodyType::Static,
                            materialRef.id, glm::vec3{0.0f});

            scene.AddScene("assets/test/loonar-test-scene.gltf");

            lua.Run("scripts/test.lua");
            core.SetKeyEventCallback(std::bind(KeyEvent, std::placeholders::_1,
                                               std::placeholders::_2,
                                               scene.GetEntities()));
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
            cam.data->SetPosition(glm::vec3(10.0f * cos(frame * 0.003f),
                                            4.5f + 2.0f * sin(frame * 0.008f),
                                            10.0f * sin(frame * 0.003f)));
            cam.data->SetProjection();

            for (auto& entity : scene.GetEntities()) {
                // Start a rendering pass for every entity
                renderer.BeginPass(0);
                cam.data->SetViewTransform(0);
                entity.second->SetVertexBuffer();
                entity.second->SetIndexBuffer();
                entity.second->ApplyTransform();
                auto matId = entity.second->GetMaterialId();
                auto mat = scene.GetMaterial(matId);
                auto albedo = scene.GetTexture(mat.data->GetAlbedoId());
                auto normal = scene.GetTexture(mat.data->GetNormalId());
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
