#include "PhysicsCore.hpp"

#include "Jolt/Core/Factory.h"
#include "Jolt/Core/JobSystemThreadPool.h"
#include "Jolt/Core/Memory.h"
#include "Jolt/RegisterTypes.h"
#include "bx/debug.h"

JPH::JobSystemThreadPool* PhysicsCore::jobSystem = nullptr;

PhysicsCore::PhysicsCore() {}

PhysicsCore::~PhysicsCore() {}

void PhysicsCore::Init() {
    JPH::RegisterDefaultAllocator();
    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();

    jobSystem = new JPH::JobSystemThreadPool(4, 16, 1024);

    system.Init(1024, 0, 1024, 1024, broadPhaseLayerInterface,
                objectVsBroadPhaseLayerFilter, objectLayerPairFilter);
    bx::debugPrintf("PhysicsCore initialized\n");
}

void PhysicsCore::Shutdown() {
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
    JPH::UnregisterTypes();
    delete jobSystem;
    bx::debugPrintf("PhysicsCore shutdown\n");
}

void PhysicsCore::Update(double deltaTime) {
    static JPH::TempAllocatorImpl tempAllocator(10 * 1024 * 1024);

    system.Update(deltaTime, 1, &tempAllocator, jobSystem);
}
