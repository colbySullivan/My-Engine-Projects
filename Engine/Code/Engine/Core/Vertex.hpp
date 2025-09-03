#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"

struct Vertex
{
public:
	Vec3 m_position;
	Rgba8 m_color;
	Vec2 m_uvTexCoords;

public:
	Vertex() {};
	explicit Vertex(Vec3 const& position, Rgba8 const& color, Vec2 const& uvTexCoords);

};