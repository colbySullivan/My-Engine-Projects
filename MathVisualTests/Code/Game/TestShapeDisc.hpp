#pragma once
#include "Game/TestShape.hpp"
#include "Game/Game.hpp"

//-----------------------------------------------------------------------------------------------
class TestShapeDisc : public TestShape
{
public:
	TestShapeDisc(Vec2 center, float radius, Rgba8 color);

	void Update() override;
	void Render() const override;

	std::vector<Vertex> m_myVerts;
	bool m_isPhysicsSimulated = false;

public:
	Vec2 m_center;;
	float m_discRadius;
	Vec2 m_mousePoint;
	Rgba8 m_color;
	Vec2 m_velocity = Vec2( 0.f, 0.f );

	void GetClosestPoint(Vec2 pointPos) override;
	bool IsPointInsideMe(Vec2 point) const override;
	void AddVertsForMe(std::vector<Vertex>& verts) const override;
	void ChangeColor(Rgba8 newColor) override;
};