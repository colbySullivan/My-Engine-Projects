#pragma once
#include "Game/TestShape.hpp"
#include "Game/Game.hpp"

//-----------------------------------------------------------------------------------------------
class TestShapeLine : public TestShape
{
public:
	TestShapeLine( Vec2 start, Vec2 end, Vec2 thickness, Rgba8 color );

	void Update() override;
	void Render() const override;

	std::vector<Vertex> m_lineVerts;

public:
	Vec2 m_start;
	Vec2 m_end;
	Vec2 m_thickness;
	Rgba8 m_color;

	Vec2 GetClosestPoint( Vec2 pointPos ) override;
	bool IsPointInsideMe( Vec2 point ) const override;
	void AddVertsForMe( std::vector<Vertex>& verts ) const override;
};