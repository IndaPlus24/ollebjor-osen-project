#pragma once

#include <Jolt/Jolt.h>
#include "Jolt/Core/JobSystemThreadPool.h"
#include "Jolt/Physics/PhysicsSystem.h"

class BroadPhaseLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
{
public:
    BroadPhaseLayerInterfaceImpl() { }

    inline uint32_t GetNumBroadPhaseLayers() const override { return 1; }
    inline JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override { return JPH::BroadPhaseLayer(0); }
#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    inline const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override { return "Default"; }
#endif
};

class PhysicsCore {
  private:
    JPH::PhysicsSystem system;
    BroadPhaseLayerInterfaceImpl broadPhaseLayerInterface;
    JPH::ObjectVsBroadPhaseLayerFilter objectVsBroadPhaseLayerFilter;
    JPH::ObjectLayerPairFilter objectLayerPairFilter;
    static JPH::JobSystemThreadPool* jobSystem;

  public:
    PhysicsCore();
    ~PhysicsCore();

    void Init();
    void Update(double deltaTime);
    void Shutdown();

    inline JPH::PhysicsSystem& GetSystem() { return system; }
};
