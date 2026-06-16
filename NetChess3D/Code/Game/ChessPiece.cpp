#include "Game/ChessPiece.hpp"
#include "Engine/Core/Engine.hpp"

//-----------------------------------------------------------------------------------------------
ChessPiece::ChessPiece( Game* owner, ChessPieceDefinition const* definition, Vec3 const& position, int playernum )
	: m_game( owner )
	, m_definition( definition )
	, m_position( position )
	, m_playernum( playernum )
{
	if ( playernum == 2 ) // #todo this needs to be handled easier
	{
		m_color = Rgba8( 120, 120, 120 );
	}
	m_effectConstant = g_engine->m_render->CreateConstantBuffer( sizeof( EffectConstants ) );
	IntVec2 square = m_game->m_chessBoard->GetSquareFromWorldPosition( m_position );
	m_game->m_chessBoard->SetPieceAt( square.y, square.x, this );
}

//-----------------------------------------------------------------------------------------------
ChessPiece::~ChessPiece()
{
	delete m_moveTimer;
	m_moveTimer = nullptr;
	delete m_effectConstant;
	m_effectConstant = nullptr;
	IntVec2 square = m_game->m_chessBoard->GetSquareFromWorldPosition( m_position );
	m_game->m_chessBoard->SetPieceAt( square.y, square.x, nullptr );
}

//-----------------------------------------------------------------------------------------------
void ChessPiece::Update()
{
	UpdateMovePiece();
	UpdateHighlight();
}

//-----------------------------------------------------------------------------------------------
void ChessPiece::Render() const
{
	if ( !m_definition || !m_definition->m_vboPlayerOne || !m_definition->m_vboPlayerTwo )
	{
		return;
	}
	Mat44 modelMatrix = Mat44::MakeTranslation3D( m_position );
	g_engine->m_render->SetModelConstants( modelMatrix, m_color );
	g_engine->m_render->m_desiredBlendMode = BlendMode::OPAQUE;
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	g_engine->m_render->BindShader( m_definition->m_shader );
	BindEffectConstant();

	if ( m_definition->m_iboPlayerOne && m_definition->m_iboPlayerTwo )
	{
		VertexBuffer* playerVbo = m_definition->m_vboPlayerOne;
		IndexBuffer* playerIbo = m_definition->m_iboPlayerOne;
		if ( m_playernum == 1 )
		{
			g_engine->m_render->BindTexture( m_definition->m_texturePlayerOne, 0 );
			g_engine->m_render->BindTexture( m_definition->m_normalTexturePlayerOne, 1 );
		}
		else if ( m_playernum == 2 )
		{
			g_engine->m_render->BindTexture( m_definition->m_texturePlayerTwo, 0 );
			g_engine->m_render->BindTexture( m_definition->m_normalTexturePlayerTwo, 1 );

			playerVbo = m_definition->m_vboPlayerTwo;
			playerIbo = m_definition->m_iboPlayerTwo;
		}

		g_engine->m_render->DrawIndexBuffer( playerVbo, playerIbo, ( unsigned int )m_definition->m_indexes.size() );
	}
	else
	{
		g_engine->m_render->DrawVertexBuffer( m_definition->m_vboPlayerOne, ( unsigned int )m_definition->m_vertexes.size() );
		g_engine->m_render->DrawVertexBuffer( m_definition->m_vboPlayerTwo, ( unsigned int )m_definition->m_vertexes.size() );
	}
	g_engine->m_render->BindShader( nullptr );
}

//-----------------------------------------------------------------------------------------------
void ChessPiece::StartMove( Vec3 const& targetWorldPosition, MoveStyle style, float durationSeconds /*= 0.5f */ )
{
	if ( m_moveTimer == nullptr )
	{
		m_moveTimer = new Timer( durationSeconds );
		m_moveTimer->Start();
	}
	else
	{
		delete m_moveTimer;
		m_moveTimer = new Timer( durationSeconds );
		m_moveTimer->Start();
	}
	m_moveStart = m_position;
	m_moveEnd = targetWorldPosition;
	m_moveStyle = style;
}

//-----------------------------------------------------------------------------------------------
void ChessPiece::BindEffectConstant() const
{
	g_engine->m_render->BindConstantBuffer( 8, m_effectConstant );
	g_engine->m_render->CopyCPUToGPU( &m_effectConstantValues, sizeof( EffectConstants ), m_effectConstant );
}

//-----------------------------------------------------------------------------------------------
void ChessPiece::UpdateMovePiece()
{
	if ( m_moveTimer && !m_moveTimer->DecrementPeriodIfElapsed() )
	{
		float t = ( float )m_moveTimer->GetElapsedFraction();
		if ( m_moveStyle == MoveStyle::Slide )
		{
			float eased = SmoothStep5( t );
			m_position = Interpolate( m_moveStart, m_moveEnd, eased );
		}
		else if ( m_moveStyle == MoveStyle::Hop )
		{
			float horizontalProgress = SmoothStep3( t );
			Vec3 base = Interpolate( m_moveStart, m_moveEnd, horizontalProgress );

			const float hopHeight = 0.6f;
			float arc = 4.0f * t * ( 1.0f - t ) * hopHeight;

			m_position = base + Vec3( 0.f, 0.f, arc );
		}
		else
		{
			m_position = m_moveEnd;
		}
	}
	else
	{
		delete m_moveTimer;
		m_moveTimer = nullptr;
	}
}

void ChessPiece::UpdateHighlight()
{
	if ( g_engine->m_input->IsKeyDown( KEYCODE_LEFT_MOUSE ) )
	{
		m_selected = ( m_currentlyRaycasted ) ? true : false;
	}

	if ( m_selected )
	{
		m_effectConstantValues.effectInt = 2;
		m_game->m_chessBoard->m_selectedPiece = this;
	}
	else if ( m_currentlyRaycasted )
	{
		m_effectConstantValues.effectInt = 1;
	}
	else
	{
		m_effectConstantValues.effectInt = 0;
	}
	m_currentlyRaycasted = false;
}
