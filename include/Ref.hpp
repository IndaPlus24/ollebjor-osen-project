#pragma once

#include "Camera.hpp"
#include "Primitive.hpp"
#include <cstdint>

template <typename T> struct SceneRef {
    uint64_t id;
    T* data;
};

struct PrimitiveRef {
    uint64_t id;
    Primitive* data;
};

struct CameraRef {
    uint64_t id;
    Camera* data;
};
