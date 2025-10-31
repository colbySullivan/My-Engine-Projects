#pragma once
#include "Game/TestShape.hpp"
#include "Game/Game.hpp"

//-----------------------------------------------------------------------------------------------
class TestShapeLine : public TestShape
{
public:
	TestShapeLine( Vec2 start, Vec2 end, Vec2 thickness, Rgba8 color );

	std::vector<Vertex> m_lineVerts;

public:
	Vec2 m_start;
	Vec2 m_end;
	Vec2 m_thickness;
	Rgba8 m_color;
};