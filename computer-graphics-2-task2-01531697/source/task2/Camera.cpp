#include "Camera.h"

Camera::Camera(OrbitalNavigator& _navigator, float near_plane, float far_plane, float fov, int viewport_width, int viewport_height) : navigator(_navigator), near_plane(near_plane), far_plane(far_plane), fov(fov)
{
  setViewport(viewport_width, viewport_height);
}

OrbitalNavigator& Camera::getNavigator()
{
	return navigator;
}

void Camera::setViewport(int viewport_width, int viewport_height)
{
	this->viewport_width = std::max(0, viewport_width);
	this->viewport_height = std::max(0, viewport_height);
  ratio = static_cast<float> (viewport_width) / static_cast<float> (viewport_height);
}

int2 Camera::getViewport() const
{
	return int2(viewport_width, viewport_height);
}

float Camera::getFOV() const
{
	return fov;
}

float Camera::getFocalLength() const
{
	return near_plane;
}

const float3& Camera::getU() const
{
	return navigator.u;
}

const float3& Camera::getV() const
{
	return navigator.v;
}

const float3& Camera::getW() const
{
	return navigator.w;
}

const float3& Camera::getPosition() const
{
	return navigator.position;
}

float3 Camera::getDirection() const
{
  return normalize(navigator.lookat - navigator.position);
}

float4x4 Camera::getView() const
{
  float3 u = getU();
  float3 v = getV();
  float3 w = -getW();
  float3 position = getPosition();
  return math::affine_float4x4( u.x, u.y, u.z, -dot(u, position),
                                v.x, v.y, v.z, -dot(v, position),
                                w.x, w.y, w.z, -dot(w, position));
}

float4x4 Camera::getProjection() const
{
  const float s2 = 1.0f / std::tan((fov * 3.1415f / 180.f) * 0.5f);
  const float s1 = s2 / ratio;
  const float z1 = (far_plane + near_plane) / (far_plane - near_plane);
  const float z2 = -2.0f * near_plane * far_plane / (far_plane - near_plane);
  return math::float4x4(s1, 0.0f, 0.0f, 0.0f,
                        0.0f, s2, 0.0f, 0.0f,
                        0.0f, 0.0f, z1, z2,
                        0.0f, 0.0f, 1.0f, 0.0f);
}
