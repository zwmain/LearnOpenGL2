# Quaternion-based Camera Design
Date: 2026-06-26
Topic: Camera System (Quaternion-based)

## Overview
This document describes the design for a camera class intended for use in an OpenGL project. The goal is to provide a 360-degree rotation system that avoids gimbal lock by using quaternions as the primary orientation state, while still providing a "First-Person" feel with pitch clamping.

## Data Model
The camera state is defined by the following primary members:
- `glm::vec3 m_position`: The camera's coordinates in world space.
- `glm::quat m_orientation`: The master rotation state.
- `glm::vec3 m_front`: The forward-facing unit vector (derived from orientation).
- `glm::vec3 m_right`: The right-hand unit vector (derived from orientation).
- `glm::vec3 m_up`: The upward unit vector (derived from orientation).

## Functional API

### Rotation & Movement
- `void Rotate(const glm::vec3& axis, float angleDegrees)`:
    - Converts `angleDegrees` to radians.
    - Creates a rotation quaternion for the specified axis.
    - Multiplies the current `m_orientation` by the new rotation.
    - Updates the cached basis vectors (`front`, `right`, `up`).
    - **Pitch Clamping**: Specifically for the Pitch axis (X-axis rotation), the resulting orientation is checked against a maximum pitch limit (e.g., 95 degrees). If exceeded, the orientation is adjusted to the limit.

- `void Move(const glm::vec3& direction, float deltaTime)`:
    - Updates `m_position` based on a provided direction vector (usually the `front` vector for First-Person movement) and a time delta.

### Targets & Interpolation
- `void LookAt(const glm::vec3& target)`:
    - Calculates the rotation required to face the `target` point.
    - Uses `glm::quatLookAt` to ensure a consistent "up" vector.
- `void SlerpTo(const Camera& targetCamera, float alpha)`:
    - Performs Spherical Linear Interpolation between the current state and a target camera's state.

### Matrix Generation
- `glm::mat4 GetViewMatrix() const`: Returns the final view matrix for the OpenGL pipeline.
- `glm::mat4 GetProjectionMatrix(float fov, float aspect, float nearPlane, float farPlane) const`: Returns the perspective projection matrix.

## Implementation Notes
- **Math Library**: GLM (OpenGL Mathematics).
- **Optimization**: Basis vectors are only recalculated when `Rotate` or `LookAt` is called to save cycles in the render loop.
- **Gimbal Lock**: By using quaternion multiplication and avoiding intermediate Euler angle storage for the primary rotation, we eliminate gimbal lock issues.
