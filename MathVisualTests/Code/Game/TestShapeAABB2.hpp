#pragma once
#include "Game/TestShape.hpp"
#include "Game/Game.hpp"

//-----------------------------------------------------------------------------------------------
class TestShapeAABB2 : public TestShape
{
public:
	TestShapeAABB2( Vec2 mins, Vec2 maxs, Rgba8 color );
	~TestShapeAABB2();

	void Update() override;
	void Render() const override;

	std::vector<Vertex> m_lineVerts;

public:
	Vec2 m_mins;
	Vec2 m_maxs;
	Rgba8 m_color;

	AABB2* m_aabb2Box = nullptr;

	void GetClosestPoint( Vec2 pointPos ) override;
	bool IsPointInsideMe( Vec2 point ) const override;
	void AddVertsForMe( std::vector<Vertex>& verts ) const override;
	void ChangeColor( Rgba8 newColor ) override;
};