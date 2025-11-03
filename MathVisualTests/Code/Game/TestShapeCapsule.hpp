#pragma once
#include "Game/TestShape.hpp"
#include "Game/Game.hpp"

//-----------------------------------------------------------------------------------------------
class TestShapeCapsule : public TestShape
{
public:
	TestShapeCapsule( Vec2 boneStart, Vec2 boneEnd, float radius, Rgba8 color );
	~TestShapeCapsule();

	void Update() override;
	void Render() const override;

	std::vector<Vertex> m_lineVerts;

public:
	Vec2 m_boneStart;
	Vec2 m_boneEnd;
	float m_radius;
	Rgba8 m_color;
	Vec2 m_mousePoint;
	AABB2* m_aabb2Box = nullptr;

	void GetClosestPoint( Vec2 pointPos ) override;
	bool IsPointInsideMe( Vec2 point ) const override;
	void AddVertsForMe( std::vector<Vertex>& verts ) const override;
	void ChangeColor( Rgba8 newColor ) override;
};