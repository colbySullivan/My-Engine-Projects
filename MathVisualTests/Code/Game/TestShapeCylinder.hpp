#pragma once
#include "Game/TestShape3D.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include <vector>

class TestShapeCylinder : public TestShape3D
{
public:
	TestShapeCylinder( const Vec3& start, const Vec3& end, float radius, const Rgba8& color = Rgba8( 255, 255, 255 ), const AABB2& UVs = AABB2::ZERO_TO_ONE, int numSlices = 32);
	~TestShapeCylinder();

	void Render() const override;
	void RenderWithTexture( Texture* texture ) const override;

	bool DoesSphereOverlap( Vec3 sphereCenter, float radius ) const override;
	bool DoesCylinderOverlap( Vec2 cylinderCenter, float cylinderRadius, FloatRange cylinderZRange ) const override;

	float m_radius;
	Vec3 m_center;
	FloatRange m_zRange;

	std::vector<Vertex> m_cylinderVerts;
};

