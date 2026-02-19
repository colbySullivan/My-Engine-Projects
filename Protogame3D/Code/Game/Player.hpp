#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex.hpp"

//------------------------------------------------------------------------------

class Player : public Entity
{
public:
	Player( Game* owner );
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;

	std::vector<Vertex> m_vertexes;
	Rgba8				m_color = Rgba8( 255, 255, 255 );
	Texture*			m_texture = nullptr;
};