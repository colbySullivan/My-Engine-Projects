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

	float m_radius;
	Vec3 m_center;

	std::vector<Vertex>	m_aabb3Verts;
};

