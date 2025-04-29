#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include "Jolt/Physics/Body/BodyActivationListener.h"
#include "Jolt/Physics/Body/BodyID.h"
#include "Jolt/Physics/Collision/Shape/Shape.h"
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

namespace Layers {
static constexpr JPH::ObjectLayer NON_MOVING = 0;
static constexpr JPH::ObjectLayer MOVING = 1;
static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
} // namespace Layers

class BroadPhaseLayers {
  public:
    enum Type : JPH::uint8 { NON_MOVING, MOVING, NUM_LAYERS };
};

class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter {
  public:
    virtual bool ShouldCollide(JPH::ObjectLayer objectLayer,
                               JPH::ObjectLayer broadPhaseLayer) const override;
};

class ObjectVsBroadPhaseLayerFilterImpl
    : public JPH::ObjectVsBroadPhaseLayerFilter {
  public:
    virtual bool
    ShouldCollide(JPH::ObjectLayer objectLayer,
                  JPH::BroadPhaseLayer broadPhaseLayer) const override;
};

class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface {
  public:
    BPLayerInterfaceImpl();

    virtual JPH::uint GetNumBroadPhaseLayers() const override;
    virtual JPH::BroadPhaseLayer
    GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override;

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    virtual const char*
    GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override;
#endif

  private:
    JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

class MyContactListener : public JPH::ContactListener {
  public:
    // See: ContactListener
    virtual JPH::ValidateResult OnContactValidate(
        const JPH::Body& inBody1, const JPH::Body& inBody2,
        JPH::RVec3Arg inBaseOffset,
        const JPH::CollideShapeResult& inCollisionResult) override;

    virtual void OnContactAdded(const JPH::Body& inBody1,
                                const JPH::Body& inBody2,
                                const JPH::ContactManifold& inManifold,
                                JPH::ContactSettings& ioSettings) override;

    virtual void OnContactPersisted(const JPH::Body& inBody1,
                                    const JPH::Body& inBody2,
                                    const JPH::ContactManifold& inManifold,
                                    JPH::ContactSettings& ioSettings) override;

    virtual void
    OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override;
};

// An example activation listener
class MyBodyActivationListener : public JPH::BodyActivationListener {
  public:
    virtual void OnBodyActivated(const JPH::BodyID& inBodyID,
                                 JPH::uint64 inBodyUserData) override;

    virtual void OnBodyDeactivated(const JPH::BodyID& inBodyID,
                                   JPH::uint64 inBodyUserData) override;
};

class PhysicsCore {
  private:
    JPH::JobSystemThreadPool* jobSystem;
    JPH::TempAllocatorImpl* tempAllocator;
    JPH::PhysicsSystem* physicsSystem;
    BPLayerInterfaceImpl* broadPhaseLayerInterface;
    const ObjectLayerPairFilterImpl* objectLayerPairFilter;
    ObjectVsBroadPhaseLayerFilterImpl* objectVsBroadPhaseLayerFilter;
    JPH::BodyActivationListener* bodyActivationListener;
    JPH::ContactListener* contactListener;

  public:
    PhysicsCore();
    ~PhysicsCore();

    void Init();
    void Update(float deltaTime);
    JPH::BodyID AddStaticBox(const JPH::Vec3& position,
                             const JPH::Vec3& halfExtent);
    JPH::BodyID AddDynamicBox(const JPH::Vec3& position,
                              const JPH::Vec3& halfExtent, float mass);
    JPH::BodyID AddDynamicSphere(float radius, JPH::RVec3 position,
                                 float mass = 1.0f);
    JPH::BodyID AddStaticPlane(const JPH::Vec3& position,
                               const JPH::Vec3& normal);

    JPH::BodyID AddStaticCollider(const JPH::Vec3& position,
                                  const JPH::Ref<JPH::Shape>& shape);

    JPH::BodyID AddDynamicCollider(const JPH::Vec3& position,
                                   const JPH::Ref<JPH::Shape> shape, float mass);

    inline JPH::BodyID RemoveBody(JPH::BodyID bodyID) {
        JPH::BodyInterface& bodyInterface = physicsSystem->GetBodyInterface();
        bodyInterface.RemoveBody(bodyID);
        return bodyID;
    }

    inline JPH::BodyInterface& GetBodyInterface() {
        return physicsSystem->GetBodyInterface();
    }
    inline JPH::PhysicsSystem& GetSystem() { return *physicsSystem; }

    void Shutdown();
};
