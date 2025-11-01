#pragma once
#include "Game/TestShape.hpp"
#include "Game/Game.hpp"

//-----------------------------------------------------------------------------------------------
class TestShapeOBB2 : public TestShape
{
public:
	TestShapeOBB2( Vec2 const& center, Vec2 const& iBasisNormal, Vec2 const& halfDimensions, Rgba8 color );

	void Update() override;
	void Render() const override;

public:
	std::vector<Vertex> m_lineVerts;
	Vec2 m_center;
	Vec2 m_iBasisNormal;
	Vec2 m_halfDimensions;
	Rgba8 m_color;

	OBB2* m_orientedBox = nullptr;

	void GetClosestPoint( Vec2 pointPos ) override;
	bool IsPointInsideMe( Vec2 point ) const override;
	void AddVertsForMe( std::vector<Vertex>& verts ) const override;
	void ChangeColor(Rgba8 newColor) override;
};