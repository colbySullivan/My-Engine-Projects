#pragma once
#include "Game/TestShape3D.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <vector>

class TestShapeCylinder : public TestShape3D
{
public:
	TestShapeCylinder( );
	~TestShapeCylinder();

	void Render() const override;

	float m_radius;
	Vec3 m_center;

	std::vector<Vertex> m_sphereVerts;
};

