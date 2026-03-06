#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Game/Entity.hpp"
#include <vector>

//------------------------------------------------------------------------------

class Prop : public Entity
{
public:
	Prop( Game* owner );

	void Update( float deltaSeconds ) override;
	void Render() const override;

	void MakeCube( Rgba8 posX = Rgba8( 255, 255, 255 ), Rgba8 negX = Rgba8( 255, 255, 255 ), Rgba8 posY = Rgba8( 255, 255, 255 ), Rgba8 negY = Rgba8( 255, 255, 255 ), Rgba8 posZ = Rgba8( 255, 255, 255 ), Rgba8 negZ = Rgba8( 255, 255, 255 ), Vec3 scale = Vec3(1,1,1) );
	void MakeCube( Rgba8 mainColor, Vec3 scale = Vec3(1,1,1) );
	void MakeCubeAtPos( Rgba8 color, Vec3 size, Vec3 position );
	void MakeSphere( Vec3 center, float radians, int numSlices, int numStacks );
	void MakeCylinder( Vec3 start, Vec3 end, float radius, int numSlices );

	std::vector<Vertex> m_vertexes;
	Rgba8				m_color = Rgba8( 255, 255, 255 );
	Texture* m_texture = nullptr;
};