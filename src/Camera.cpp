#include "Camera.hpp"

#include "Renderer.hpp"
#include "bgfx/bgfx.h"
#include "bx/math.h"
#include "glm/ext/matrix_transform.hpp"

Camera::Camera(Renderer& renderer, const glm::vec3& position,
               const glm::vec3& rotation, const glm::vec3& up, float fov,
               float nearPlane, float farPlane)
    : renderer(renderer), position(position), up(up), fov(fov),
      nearPlane(nearPlane), farPlane(farPlane) {
    // Set the projection matrix
    SetProjection(fov, nearPlane, farPlane);
    // Set the view matrix
    LookAt(target);
}

Camera::~Camera() {}

void Camera::SetPosition(const glm::vec3& position) {
    this->position = position;
    // Update the view matrix when the position changes
    LookAt(target);
}

void Camera::SetFov(float fov) {
    this->fov = fov;
    // Update the projection matrix when the FOV changes
    SetProjection(fov, nearPlane, farPlane);
}

void Camera::SetNearPlane(float nearPlane) {
    this->nearPlane = nearPlane;
    // Update the projection matrix when the near plane changes
    SetProjection(fov, nearPlane, farPlane);
}

void Camera::SetFarPlane(float farPlane) {
    this->farPlane = farPlane;
    // Update the projection matrix when the far plane changes
    SetProjection(fov, nearPlane, farPlane);
}

void Camera::SetProjection() { SetProjection(fov, nearPlane, farPlane); }

void Camera::SetProjection(float fov, float nearPlane, float farPlane) {
    bx::mtxProj(projection, fov, renderer.GetAspectRatio(), nearPlane, farPlane,
                bgfx::getCaps()->homogeneousDepth);
}

void Camera::LookAt(const glm::vec3& target) {
    this->target = target;
    bx::Vec3 upVec = {up.x, up.y, up.z};
    bx::Vec3 positionVec = {position.x, position.y, position.z};
    bx::Vec3 targetVec = {target.x, target.y, target.z};
    bx::mtxLookAt(view, positionVec, targetVec, upVec);
}

void Camera::SetViewTransform(bgfx::ViewId viewId) {
    bgfx::setViewTransform(viewId, view, projection);
}
