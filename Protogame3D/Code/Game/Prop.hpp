#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex.hpp"

//------------------------------------------------------------------------------

class Prop : public Entity
{
public:
	Prop( Game* owner );

	void Update( float deltaSeconds ) override;
	void Render() const override;

	void MakeCube( Rgba8 posX = Rgba8( 255, 255, 255 ), Rgba8 negX = Rgba8( 255, 255, 255 ), Rgba8 posY = Rgba8( 255, 255, 255 ), Rgba8 negY = Rgba8( 255, 255, 255 ), Rgba8 posZ = Rgba8( 255, 255, 255 ), Rgba8 negZ = Rgba8( 255, 255, 255 ) );

	std::vector<Vertex> m_vertexes;
	Rgba8				m_color = Rgba8( 255, 255, 255 );
	Texture* m_texture = nullptr;
};