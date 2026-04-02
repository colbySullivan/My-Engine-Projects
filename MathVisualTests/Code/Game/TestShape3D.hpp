#pragma once
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"


struct TestShape3D
{
	TestShape3D() = default;
	virtual ~TestShape3D() = default;
	virtual void IsOverlapped() const;
	virtual void Render() const = 0;
	virtual void RenderWithTexture( Texture* texture ) const = 0;

	virtual RaycastResult3D RaycastTestShape( Vec3 startPos, Vec3 forwardNormal, float maxDistance ) const = 0;
	virtual bool DoesSphereOverlap( Vec3 sphereCenter, float radius ) const = 0;
	virtual bool DoesCylinderOverlap( Vec2 cylinderCenter, float cylinderRadius, FloatRange cylinderZRange ) const = 0;
	virtual bool DoesAABBOverlap( Vec3 aabbMins, Vec3 aabbMaxs ) const = 0;


	bool m_isOverlapping = false;

};