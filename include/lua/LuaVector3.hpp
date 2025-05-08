#include "glm/geometric.hpp"
#include <glm/glm.hpp>
#include <sstream>

// TODO: Overload metamethods so that vector3 * float and float * vector3 works
class LuaVector3 {
  public:
    static glm::vec3 __add(const glm::vec3& lhs, const glm::vec3& rhs) {
        return lhs + rhs;
    }
    static glm::vec3 __sub(const glm::vec3& lhs, const glm::vec3& rhs) {
        return lhs - rhs;
    }
    static glm::vec3 __mul(const glm::vec3& lhs, float rhs) {
        return lhs * rhs;
    }
    static glm::vec3 __div(const glm::vec3& lhs, float rhs) {
        return lhs / rhs;
    }
    static glm::vec3 __unm(const glm::vec3& lhs) { return -lhs; }
    static std::string __tostring(const glm::vec3& lhs) {
        std::ostringstream oss;
        oss << "Vector3(" << lhs.x << ", " << lhs.y << ", " << lhs.z << ")";
        return oss.str();
    }
    static bool __eq(const glm::vec3& lhs, const glm::vec3& rhs) {
        return lhs == rhs;
    }
    static float length(const glm::vec3& lhs) { return glm::length(lhs); }

    static glm::vec3 normalize(const glm::vec3& lhs) {
        return glm::normalize(lhs);
    }
    static float dot(const glm::vec3& lhs, const glm::vec3& rhs) {
        return glm::dot(lhs, rhs);
    }
    static glm::vec3 cross(const glm::vec3& lhs, const glm::vec3& rhs) {
        return glm::cross(lhs, rhs);
    }
};