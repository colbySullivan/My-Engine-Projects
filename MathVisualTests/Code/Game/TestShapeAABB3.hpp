#pragma once
#include "Game/TestShape3D.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB3.hpp"
#include <vector>

class TestShapeAABB3 : public TestShape3D
{
public:
	TestShapeAABB3( const AABB3& bounds, const Rgba8& color = Rgba8( 255, 255, 255 ), const AABB2& UVs = AABB2::ZERO_TO_ONE);
	~TestShapeAABB3();


	void Render() const override;
	void RenderWithTexture( Texture* texture ) const override;

	RaycastResult3D RaycastTestShape( Vec3 startPos, Vec3 forwardNormal, float maxDistance ) const override;
	bool DoesSphereOverlap( Vec3 sphereCenter, float radius ) const override;
	bool DoesCylinderOverlap( Vec2 cylinderCenter, float cylinderRadius, FloatRange cylinderZRange ) const override;
	bool DoesAABBOverlap( Vec3 aabbMins, Vec3 aabbMaxs ) const override;

	AABB3 m_bounds;

	std::vector<Vertex>	m_aabb3Verts;
};

