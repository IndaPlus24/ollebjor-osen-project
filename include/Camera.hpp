#pragma once

#include "Renderer.hpp"
#include <glm/glm.hpp>

class Camera {
  private:
    float view[16];
    float projection[16];

    glm::vec3 position;
    glm::vec3 up;
    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);

    float fov;
    float nearPlane;
    float farPlane;
    Renderer& renderer;

    void SetProjection(float fov, float nearPlane, float farPlane);

  public:
    Camera(Renderer& renderer, const glm::vec3& position, const glm::vec3& up,
           float fov, float nearPlane, float farPlane);
    Camera(Camera&& other) noexcept;
    Camera& operator=(Camera&& other) noexcept;
    ~Camera();

    inline const glm::vec3& GetPosition() const { return position; }

    void SetPosition(const glm::vec3& position);

    void SetFov(float fov);
    void SetNearPlane(float nearPlane);
    void SetFarPlane(float farPlane);

    void SetProjection();
    void SetViewTransform(bgfx::ViewId viewId = 0);

    void LookAt(const glm::vec3& target);
};
