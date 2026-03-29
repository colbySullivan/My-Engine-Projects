#pragma once
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/Vec3.hpp"


struct TestShape3D
{
	TestShape3D() = default;
	virtual ~TestShape3D() = default;
	virtual void IsOverlapped() const;
	virtual void Render() const = 0;
	virtual void RenderWithTexture( Texture* texture ) const = 0;

	virtual bool DoesSphereOverlap( Vec3 sphereCenter, float radius ) const = 0;

	bool m_isOverlapping = false;

};