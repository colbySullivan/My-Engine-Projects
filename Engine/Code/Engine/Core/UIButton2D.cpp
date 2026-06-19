#include "Engine/Core/UIButton2D.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine.hpp"

//-----------------------------------------------------------------------------------------------
UIButton2D::UIButton2D( const Vec2& min, const Vec2& max, const std::string& buttonText, const std::string& onClickEvent, Rgba8 baseColor, Rgba8 hoveredColor )
	: m_bounds( AABB2( min, max ) )
	, m_buttonText( buttonText )
	, m_baseColor( baseColor )
	, m_hoveredColor( hoveredColor )
	, m_isCurrentlyHovered( false )
	, m_onClickEvent( onClickEvent )
{
	m_font = g_engine->m_render->CreateOrGetBitmapFont( "Data/Fonts/SquirrelFixedFont");
}

//-----------------------------------------------------------------------------------------------
UIButton2D::UIButton2D( const Vec2& centerPosition, float sizeX, float sizeY, const std::string& buttonText, const std::string& onClickEvent, Rgba8 baseColor, Rgba8 hoveredColor )
	: m_buttonText( buttonText )
	, m_baseColor( baseColor )
	, m_hoveredColor( hoveredColor )
	, m_isCurrentlyHovered( false )
	, m_onClickEvent( onClickEvent )
{
	float halfX = sizeX * 0.5f;
	float halfY = sizeY * 0.5f;

	m_bounds = AABB2(
		Vec2( centerPosition.x - halfX, centerPosition.y - halfY ),
		Vec2( centerPosition.x + halfX, centerPosition.y + halfY )
	);
	m_font = g_engine->m_render->CreateOrGetBitmapFont( "Data/Fonts/SquirrelFixedFont" );
}

//-----------------------------------------------------------------------------------------------
void UIButton2D::Update( const Vec2& mousePos, bool isMouseLeftJustPressed )
{
	m_isCurrentlyHovered = m_bounds.IsPointInside( mousePos );

	if ( m_isCurrentlyHovered && isMouseLeftJustPressed )
	{
		if ( !m_onClickEvent.empty() )
		{
			FireEvent( m_onClickEvent, m_eventArgs );
		}
	}
}

//-----------------------------------------------------------------------------------------------
void UIButton2D::Render() const
{
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	g_engine->m_render->BindTexture( nullptr );

	std::vector<Vertex> boxVerts;
	Rgba8 color = ( m_isCurrentlyHovered ) ? m_hoveredColor : m_baseColor;
	Texture* texture = ( m_isCurrentlyHovered ) ? m_hoveredTexture : m_defaultTexture;
	AddVertsForAABB2D( boxVerts, m_bounds, color );
	g_engine->m_render->BindTexture( texture );
	g_engine->m_render->DrawVertexArray( boxVerts );

	std::vector<Vertex> textVerts;
	m_font->AddVertsForTextInBox2D( textVerts, m_buttonText, m_bounds, 15.f );
	g_engine->m_render->BindTexture( &m_font->GetTexture() );
	g_engine->m_render->DrawVertexArray( textVerts );
}

//-----------------------------------------------------------------------------------------------
void UIButton2D::UpdateHoverState( const Vec2& mousePos )
{
	m_isCurrentlyHovered = m_bounds.IsPointInside( mousePos );
}

//-----------------------------------------------------------------------------------------------
bool UIButton2D::IsHovered() const
{
	return m_isCurrentlyHovered;
}

//-----------------------------------------------------------------------------------------------
void UIButton2D::AddArg( std::string argValue, std::string argName )
{
	m_eventArgs.SetValue( argValue, argName );
}
