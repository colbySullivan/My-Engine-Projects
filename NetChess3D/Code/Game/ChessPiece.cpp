#include "Game/ChessPiece.hpp"
#include "Engine/Core/Engine.hpp"

//-----------------------------------------------------------------------------------------------
ChessPiece::ChessPiece( Game* owner, ChessPieceDefinition const* definition, Vec3 const& position, PlayerNumber playernum )
	: m_game( owner )
	, m_definition( definition )
	, m_position( position )
{
	if ( playernum == PlayerTwo ) // #todo this needs to be handled easier
	{
		m_color = Rgba8( 120, 120, 120 );
	}
}

//-----------------------------------------------------------------------------------------------
ChessPiece::~ChessPiece()
{

}

//-----------------------------------------------------------------------------------------------
void ChessPiece::Update()
{

}

//-----------------------------------------------------------------------------------------------
void ChessPiece::Render() const
{
	if ( !m_definition || !m_definition->m_vbo )
	{
		return;
	}
	Mat44 modelMatrix = Mat44::MakeTranslation3D( m_position );
	g_engine->m_render->SetModelConstants( modelMatrix, m_color );
	g_engine->m_render->m_desiredBlendMode = BlendMode::OPAQUE;
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	g_engine->m_render->DrawVertexBuffer( m_definition->m_vbo, ( unsigned int )m_definition->m_vertexes.size() );
}

