#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec2 texCoord;

    inline bool operator==(const Vertex& other) const {
        return pos == other.pos && normal == other.normal &&
               texCoord == other.texCoord && tangent == other.tangent;
    }
};
