#pragma once
#include "Engine/Core/VertexUtils.hpp"

//-----------------------------------------------------------------------------------------------
class TestShape
{
public:
	TestShape();
	virtual ~TestShape();

	virtual void Update();
	virtual void Render() const;

	virtual Vec2 GetClosestPoint( Vec2 pointPos );

public:
	std::vector<Vertex> m_pointVerts;
	Vec2 m_closestPoint;
};