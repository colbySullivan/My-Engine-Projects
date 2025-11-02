#pragma once
#include "Game/TestShape.hpp"
#include "Game/Game.hpp"

//-----------------------------------------------------------------------------------------------
class TestShapeTriangle : public TestShape
{
public:
	TestShapeTriangle( Vec2 ccw0, Vec2 ccw1, Vec2 ccw2, Rgba8 color );

	void Update() override;
	void Render() const override;

public:
	std::vector<Vertex> m_lineVerts;
	Vec2 m_aPoint;
	Vec2 m_bPoint;
	Vec2 m_cPoint;
	Rgba8 m_color;
	Vec2 m_mousePoint;

	void GetClosestPoint( Vec2 pointPos ) override;
	bool IsPointInsideMe( Vec2 point ) const override;
	void AddVertsForMe( std::vector<Vertex>& verts ) const override;
	void ChangeColor( Rgba8 newColor ) override;
};