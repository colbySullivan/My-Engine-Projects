#include "Vertex.hpp"

Vertex::Vertex(Vec3 const& position, Rgba8 const& color, Vec2 const& uvTexCoords)
	: m_position(position)
	, m_color(color)
	, m_uvTexCoords(uvTexCoords)
{
}
