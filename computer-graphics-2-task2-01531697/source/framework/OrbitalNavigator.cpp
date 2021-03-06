#include "OrbitalNavigator.h"

OrbitalNavigator::OrbitalNavigator(float phi, float theta, float radius, const math::float3& lookat)
	: drag(0U),
		initial_lookat(lookat),
		initial_phi(phi),
		initial_theta(theta),
		initial_radius(radius),
		phi(phi),
	  theta(theta),
	  radius(radius),
	  lookat(lookat),
    changed(false)
{
	update();
}

void OrbitalNavigator::rotateH(float dphi)
{
	phi = math::fmod(phi + dphi, 2.0f * math::constants<float>::pi());
}

void OrbitalNavigator::rotateV(float dtheta)
{
	theta = math::fmod(theta + dtheta, 2.0f * math::constants<float>::pi());
}

void OrbitalNavigator::zoom(float dr)
{
	radius += dr;
}

void OrbitalNavigator::pan(float x, float y)
{
	lookat += x * u + y * v;
}

void OrbitalNavigator::update()
{
	float cp = math::cos(phi);
	float sp = math::sin(phi);
	float ct = math::cos(theta);
	float st = math::sin(theta);

	w = math::float3(ct * cp, st, ct * sp);
	v = math::float3(-st * cp, ct, -st * sp);
	u = cross(v, -w);

	position = lookat - radius * (-w);
  changed = true;
}

bool OrbitalNavigator::hasChanged()
{
  return changed ? !(changed = false) : false;
}

void OrbitalNavigator::reset()
{
	lookat = initial_lookat;
	phi = initial_phi;
	theta = initial_theta;
	radius = initial_radius;
	update();
}

void OrbitalNavigator::writeWorldToLocalTransform(math::affine_float4x4* M) const
{
	*M = math::affine_float4x4(u.x, u.y, u.z, -dot(u, position),
	                           v.x, v.y, v.z, -dot(v, position),
	                           -w.x, -w.y, -w.z, -dot(-w, position));
}

void OrbitalNavigator::writeLocalToWorldTransform(math::affine_float4x4* M) const
{
	*M = math::affine_float4x4(u.x, v.x, -w.x, position.x,
	                           u.y, v.y, -w.y, position.z,
	                           u.z, v.z, -w.z, position.y);
}

void OrbitalNavigator::writePosition(math::float3* p) const
{
	*p = OrbitalNavigator::position;
}


void OrbitalNavigator::buttonDown(Button button, int x, int y)
{
	drag |= static_cast<unsigned int>(button);
	last_pos = math::int2(x, y);
}

void OrbitalNavigator::buttonUp(Button button, int x, int y)
{
	drag &= ~static_cast<unsigned int>(button);
}

void OrbitalNavigator::mouseMove(int x, int y)
{
	if (drag)
	{
		math::int2 pos = math::int2(x, y);
		math::int2 d = pos - last_pos;

		if (drag & static_cast<unsigned int>(Button::LEFT))
		{
			rotateH(-d.x * 0.012f);
			rotateV(d.y * 0.012f);
		}

		if (drag & static_cast<unsigned int>(Button::RIGHT))
		{
			math::int2 absd = abs(d);

			float dr = ((absd.y > absd.x) ? (d.y < 0 ? 1.0f : -1.0f) : (d.x > 0.0f ? 1.0f : -1.0f)) * math::sqrt(static_cast<float>(d.x * d.x + d.y * d.y)) * 0.012f * radius;
			if (dr > 0.0f && dr < 0.0001f) {
				dr = 0.005f;
			}
			else if (dr < 0.0f && dr > 0.0001f) {
				dr = -0.005f;
			}
			zoom(dr);
		}

		if (drag & static_cast<unsigned int>(Button::MIDDLE))
		{
			pan(-d.x * radius * 0.007f, d.y * radius * 0.007f);
		}

		update();

		last_pos = pos;
	}
}

void OrbitalNavigator::mouseWheel(int delta)
{
	//zoom(delta * 0.007f * radius);
  zoom(-delta * 0.025f);
	update();
}
