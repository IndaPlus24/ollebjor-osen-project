#pragma once

#include <glm/glm.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Math/Vec3.h>
#include <glm/gtc/quaternion.hpp>
#include <Jolt/Math/Quat.h>
#include <glm/gtc/matrix_transform.hpp>
#include <Jolt/Math/Mat44.h>

// Convert JPH::Vec3 to glm::vec3
inline glm::vec3 ToGLM(const JPH::Vec3& v) {
    return glm::vec3(v.GetX(), v.GetY(), v.GetZ());
}

// Convert glm::vec3 to JPH::Vec3
inline JPH::Vec3 ToJPH(const glm::vec3& v) { return JPH::Vec3(v.x, v.y, v.z); }

// Convert JPH::Vec4 to glm::vec4
inline glm::vec4 ToGLM(const JPH::Vec4& v) {
    return glm::vec4(v.GetX(), v.GetY(), v.GetZ(), v.GetW());
}

// Convert glm::vec4 to JPH::Vec4
inline JPH::Vec4 ToJPH(const glm::vec4& v) {
    return JPH::Vec4(v.x, v.y, v.z, v.w);
}

// Convert JPH::Quat to glm::quat
inline glm::quat ToGLM(const JPH::Quat& q) {
    return glm::quat(q.GetW(), q.GetX(), q.GetY(), q.GetZ());
}

// Convert glm::quat to JPH::Quat
inline JPH::Quat ToJPH(const glm::quat& q) {
    return JPH::Quat(q.x, q.y, q.z, q.w);
}

// Convert JPH::Mat44 to glm::mat4
inline glm::mat4 ToGLM(const JPH::Mat44& m) {
    return glm::mat4(ToGLM(m.GetColumn4(0)), // Column 0
                     ToGLM(m.GetColumn4(1)), // Column 1
                     ToGLM(m.GetColumn4(2)), // Column 2
                     ToGLM(m.GetColumn4(3))  // Column 3 (translation)
    );
}

// Convert glm::mat4 to JPH::Mat44
inline JPH::Mat44 ToJPH(const glm::mat4& m) {
    return JPH::Mat44(ToJPH(glm::vec4(m[0])), // Column 0
                      ToJPH(glm::vec4(m[1])), // Column 1
                      ToJPH(glm::vec4(m[2])), // Column 2
                      ToJPH(glm::vec4(m[3]))  // Column 3 (translation)
    );
}
