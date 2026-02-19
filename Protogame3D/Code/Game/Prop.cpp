#include "Game/Prop.hpp"
#include "Engine/Core/Engine.hpp"

//------------------------------------------------------------------------------
Prop::Prop( Game* owner )
	: Entity( owner )
{
}

//-----------------------------------------------------------------------------------------------
void Prop::Update( [[maybe_unused]] float deltaSeconds )
{
	m_orientation += m_angularVelocity;
}

//-----------------------------------------------------------------------------------------------
void Prop::Render() const
{
	g_engine->m_render->SetBlendMode(BlendMode::OPAQUE);
	g_engine->m_render->BindTexture( m_texture );
	g_engine->m_render->DrawVertexArray( ( int )m_vertexes.size(), m_vertexes.data() );
}
