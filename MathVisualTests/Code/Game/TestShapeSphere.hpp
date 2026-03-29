#pragma once
#include "Game/TestShape3D.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <vector>

class TestShapeSphere : public TestShape3D
{
	public:
	TestShapeSphere( Vec3 center, float radius, int numSlices, int numStacks, const Rgba8& color = Rgba8( 255, 255, 255 ) );
	~TestShapeSphere();

	void Render() const override;
	void RenderWithTexture( Texture* texture ) const override;
	
	bool DoesSphereOverlap( Vec3 sphereCenter, float radius ) const override;

	float m_radius;
	Vec3 m_center;

	std::vector<Vertex> m_sphereVerts;
};

