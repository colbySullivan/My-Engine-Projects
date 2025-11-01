#pragma once
#include "Engine/Core/VertexUtils.hpp"

//-----------------------------------------------------------------------------------------------
class TestShape
{
public:
	TestShape() = default;
	virtual ~TestShape();

	virtual void Update();
	virtual void Render() const;

	virtual bool IsPointInsideMe( Vec2 point ) const = 0;
	virtual void GetClosestPoint( Vec2 pointPos ) = 0;
	virtual void AddVertsForMe( std::vector<Vertex>& verts ) const = 0;
	virtual void ChangeColor( Rgba8 newColor ) = 0;

public:
	std::vector<Vertex> m_pointVerts;
	Vec2 m_closestPoint;
	Rgba8 m_color;
};