#include "PhysicsCore.hpp"
#include "GameEngineLogger.hpp"
#include "Jolt/Geometry/Plane.h"
#include "Jolt/Physics/Body/BodyActivationListener.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Body/BodyID.h"
#include "Jolt/Physics/Collision/ContactListener.h"
#include "Jolt/Physics/Collision/Shape/PlaneShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>

bool ObjectLayerPairFilterImpl::ShouldCollide(JPH::ObjectLayer inLayer1,
                                              JPH::ObjectLayer inLayer2) const {
    // return !(inLayer1 == Layers::MOVING && inLayer2 == Layers::MOVING);
    return true; // Collides with all object layers
}

bool ObjectVsBroadPhaseLayerFilterImpl::ShouldCollide(
    JPH::ObjectLayer objectLayer, JPH::BroadPhaseLayer broadPhaseLayer) const {
    switch (objectLayer) {
    case Layers::NON_MOVING:
        return (JPH::uint8)broadPhaseLayer == BroadPhaseLayers::MOVING;
    case Layers::MOVING:
        return true; // Collides with all broad-phase layers
    default:
        return false;
    }
}

BPLayerInterfaceImpl::BPLayerInterfaceImpl() {
    mObjectToBroadPhase[Layers::NON_MOVING] =
        (JPH::BroadPhaseLayer)BroadPhaseLayers::NON_MOVING;
    mObjectToBroadPhase[Layers::MOVING] =
        (JPH::BroadPhaseLayer)BroadPhaseLayers::MOVING;
}

JPH::uint BPLayerInterfaceImpl::GetNumBroadPhaseLayers() const {
    return BroadPhaseLayers::NUM_LAYERS;
}

JPH::BroadPhaseLayer
BPLayerInterfaceImpl::GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const {
    return mObjectToBroadPhase[inLayer];
}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
const char* BPLayerInterfaceImpl::GetBroadPhaseLayerName(
    JPH::BroadPhaseLayer inLayer) const {
    switch ((JPH::uint8)inLayer) {
    case BroadPhaseLayers::NON_MOVING:
        return "NON_MOVING";
    case BroadPhaseLayers::MOVING:
        return "MOVING";
    default:
        return "UNKNOWN";
    }
}
#endif

JPH::ValidateResult MyContactListener::OnContactValidate(
    const JPH::Body& inBody1, const JPH::Body& inBody2,
    JPH::RVec3Arg inBaseOffset,
    const JPH::CollideShapeResult& inCollisionResult) {
    LOG_DEBUG("Physics", "Contact validate callback");

    // Allows you to ignore a contact before it is created (using layers to
    // not make objects collide is cheaper!)
    return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
}

void MyContactListener::OnContactAdded(const JPH::Body& inBody1,
                                       const JPH::Body& inBody2,
                                       const JPH::ContactManifold& inManifold,
                                       JPH::ContactSettings& ioSettings) {
    LOG_DEBUG("Physics", "A contact was added");
}

void MyContactListener::OnContactPersisted(
    const JPH::Body& inBody1, const JPH::Body& inBody2,
    const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) {
    LOG_DEBUG("Physics", "A contact was persisted");
}

void MyContactListener::OnContactRemoved(
    const JPH::SubShapeIDPair& inSubShapePair) {
    LOG_DEBUG("Physics", "A contact was removed");
}

// An example activation listener
void MyBodyActivationListener::OnBodyActivated(const JPH::BodyID& inBodyID,
                                               JPH::uint64 inBodyUserData) {
    LOG_DEBUG("Physics", "A body got activated, id: " + std::to_string(inBodyID.GetIndex()));
}

void MyBodyActivationListener::OnBodyDeactivated(const JPH::BodyID& inBodyID,
                                                 JPH::uint64 inBodyUserData) {
    LOG_DEBUG("Physics", "A body went to sleep, id: " + std::to_string(inBodyID.GetIndex()));
}

PhysicsCore::PhysicsCore()
    : jobSystem(nullptr), physicsSystem(nullptr), tempAllocator(nullptr) {}
PhysicsCore::~PhysicsCore() { Shutdown(); }

void PhysicsCore::Initialize() {
    // Initialize Jolt
    JPH::RegisterDefaultAllocator();
    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();

    // Create a job system with multiple threads
    jobSystem = new JPH::JobSystemThreadPool(
        JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers,
        std::thread::hardware_concurrency() - 1);

    // Create a temporary allocator
    tempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024); // 10 MB

    // Initialize physics system with layers
    physicsSystem = new JPH::PhysicsSystem();
    broadPhaseLayerInterface = new BPLayerInterfaceImpl();
    objectLayerPairFilter = new ObjectLayerPairFilterImpl();
    objectVsBroadPhaseLayerFilter = new ObjectVsBroadPhaseLayerFilterImpl();
    contactListener = new MyContactListener();
    bodyActivationListener = new MyBodyActivationListener();

    physicsSystem->Init(1024, 0, 1024, 1024, *broadPhaseLayerInterface,
                        *objectVsBroadPhaseLayerFilter, *objectLayerPairFilter);
    physicsSystem->SetGravity(JPH::Vec3(0, -9.81f, 0));
    // physicsSystem->SetContactListener(contactListener);
    // physicsSystem->SetBodyActivationListener(bodyActivationListener);

    LOG_INFO("Physics", "Jolt Physics initialized successfully.");
}

void PhysicsCore::Update(float deltaTime) {
    if (physicsSystem) {
        physicsSystem->Update(deltaTime, 2, tempAllocator, jobSystem);
    }
}

JPH::BodyID PhysicsCore::AddStaticBox(const JPH::Vec3& position,
                                      const JPH::Vec3& halfExtent) {
    JPH::BodyInterface& bodyInterface = physicsSystem->GetBodyInterface();

    // Create a box shape
    JPH::Ref<JPH::BoxShape> boxShape = new JPH::BoxShape(halfExtent);

    // Define the body settings
    JPH::BodyCreationSettings settings(boxShape, position,
                                       JPH::Quat::sIdentity(),
                                       JPH::EMotionType::Static, 0);

    // Create the body
    JPH::BodyID bodyID;
    JPH::Body* body = bodyInterface.CreateBody(settings);
    if (body) {
        bodyID = body->GetID();
        bodyInterface.AddBody(bodyID, JPH::EActivation::DontActivate);
    }
    return JPH::BodyID(); // Return an invalid ID if body creation failed
}

JPH::BodyID PhysicsCore::AddDynamicBox(const JPH::Vec3& position,
                                       const JPH::Vec3& halfExtent,
                                       float mass) {
    if (!physicsSystem) {
        LOG_ERROR("Physics", "Physics system is not initialized!");
        return JPH::BodyID(); // Return an invalid body ID
    }

    // Get the Body Interface
    JPH::BodyInterface& bodyInterface = physicsSystem->GetBodyInterface();

    // Create a Sphere Shape
    JPH::Ref<JPH::BoxShape> boxShape = new JPH::BoxShape(halfExtent);

    // Define the body settings
    JPH::BodyCreationSettings bodySettings(
        boxShape, position, JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic,
        Layers::MOVING);

    // Set mass properties (important for dynamic bodies)
    bodySettings.mOverrideMassProperties =
        JPH::EOverrideMassProperties::CalculateInertia;
    bodySettings.mMassPropertiesOverride.mMass = mass;

    // Create the body
    JPH::BodyID bodyID;
    JPH::Body* body = bodyInterface.CreateBody(bodySettings);
    if (body) {
        bodyID = body->GetID();
        bodyInterface.AddBody(bodyID, JPH::EActivation::Activate);
    } else {
        LOG_ERROR("Physics", "Failed to create dynamic box!");
    }

    return bodyID;
}

JPH::BodyID PhysicsCore::AddDynamicSphere(float radius, JPH::RVec3 position,
                                          float mass) {
    if (!physicsSystem) {
        LOG_ERROR("Physics", "Physics system is not initialized!");
        return JPH::BodyID(); // Return an invalid body ID
    }

    // Get the Body Interface
    JPH::BodyInterface& bodyInterface = physicsSystem->GetBodyInterface();

    // Create a Sphere Shape
    JPH::Ref<JPH::Shape> sphereShape = new JPH::SphereShape(radius);

    // Define the body settings
    JPH::BodyCreationSettings bodySettings(
        sphereShape, position, JPH::Quat::sIdentity(),
        JPH::EMotionType::Dynamic, Layers::MOVING);

    // Set mass properties (important for dynamic bodies)
    bodySettings.mOverrideMassProperties =
        JPH::EOverrideMassProperties::CalculateInertia;
    bodySettings.mMassPropertiesOverride.mMass = mass;

    // Create the body
    JPH::BodyID bodyID;
    JPH::Body* body = bodyInterface.CreateBody(bodySettings);
    if (body) {
        bodyID = body->GetID();
        bodyInterface.AddBody(bodyID, JPH::EActivation::Activate);
    } else {
        LOG_ERROR("Physics", "Failed to create dynamic sphere!");
    }

    return bodyID;
}

JPH::BodyID PhysicsCore::AddStaticPlane(const JPH::Vec3& position,
                                        const JPH::Vec3& normal) {
    JPH::BodyInterface& bodyInterface = physicsSystem->GetBodyInterface();

    // Create a plane shape
    JPH::Plane planeSettinge(normal, 0);
    JPH::Ref<JPH::PlaneShape> planeShape = new JPH::PlaneShape(planeSettinge);

    // Define the body settings
    JPH::BodyCreationSettings settings(planeShape, position,
                                       JPH::Quat::sIdentity(),
                                       JPH::EMotionType::Static, 0);

    // Create the body
    JPH::BodyID bodyID;
    JPH::Body* body = bodyInterface.CreateBody(settings);
    if (body) {
        bodyID = body->GetID();
        bodyInterface.AddBody(bodyID, JPH::EActivation::DontActivate);
        return bodyID;
    }
    return JPH::BodyID(); // Return an invalid ID if body creation failed
}

JPH::BodyID PhysicsCore::AddStaticCollider(const JPH::Vec3& position,
                                           const JPH::Ref<JPH::Shape>& shape) {
    JPH::BodyInterface& bodyInterface = physicsSystem->GetBodyInterface();

    // Define the body settings
    JPH::BodyCreationSettings settings(shape, position, JPH::Quat::sIdentity(),
                                       JPH::EMotionType::Static, 0);
    // Create the body
    JPH::BodyID bodyID;
    JPH::Body* body = bodyInterface.CreateBody(settings);
    if (body) {
        bodyID = body->GetID();
        bodyInterface.AddBody(bodyID, JPH::EActivation::DontActivate);
        return bodyID;
    }
    return JPH::BodyID(); // Return an invalid ID if body creation failed
}

JPH::BodyID PhysicsCore::AddDynamicCollider(const JPH::Vec3& position,
                                            const JPH::Ref<JPH::Shape> shape,
                                            float mass) {
    JPH::BodyInterface& bodyInterface = physicsSystem->GetBodyInterface();
    // Define the body settings
    JPH::BodyCreationSettings settings(shape, position, JPH::Quat::sIdentity(),
                                       JPH::EMotionType::Dynamic, Layers::MOVING);
    // Set mass properties (important for dynamic bodies)
    settings.mOverrideMassProperties =
        JPH::EOverrideMassProperties::CalculateInertia;
    settings.mMassPropertiesOverride.mMass = mass;
    // Create the body
    JPH::BodyID bodyID;
    JPH::Body* body = bodyInterface.CreateBody(settings);
    if (body) {
        bodyID = body->GetID();
        bodyInterface.AddBody(bodyID, JPH::EActivation::Activate);
        return bodyID;
    }
    return JPH::BodyID(); // Return an invalid ID if body creation failed
}

void PhysicsCore::Shutdown() {
    if (physicsSystem) {
        delete physicsSystem;
        physicsSystem = nullptr;
    }
    if (broadPhaseLayerInterface) {
        delete broadPhaseLayerInterface;
        broadPhaseLayerInterface = nullptr;
    }
    if (objectLayerPairFilter) {
        delete objectLayerPairFilter;
        objectLayerPairFilter = nullptr;
    }
    if (objectVsBroadPhaseLayerFilter) {
        delete objectVsBroadPhaseLayerFilter;
        objectVsBroadPhaseLayerFilter = nullptr;
    }
    if (tempAllocator) {
        delete tempAllocator;
        tempAllocator = nullptr;
    }
    if (jobSystem) {
        delete jobSystem;
        jobSystem = nullptr;
    }
    if (JPH::Factory::sInstance) {
        delete JPH::Factory::sInstance;
        JPH::Factory::sInstance = nullptr;
    }
    if (contactListener) {
        delete contactListener;
        contactListener = nullptr;
    }
    if (bodyActivationListener) {
        delete bodyActivationListener;
        bodyActivationListener = nullptr;
    }
    JPH::UnregisterTypes();
    LOG_INFO("Physics", "Jolt Physics shutdown successfully.");
}
