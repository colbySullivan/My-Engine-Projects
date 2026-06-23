#include "Game/ShopItemCard.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/StringUtils.hpp"

//------------------------------------------------------------------------------
ShopItemCard::ShopItemCard( const ItemDefinitions* item, Vec2 centerPos, float sizeX, float sizeY )
	: m_item( item )
{
	float halfX = sizeX * 0.5f;
	float halfY = sizeY * 0.5f;
	m_bounds = AABB2( Vec2( centerPos.x - halfX, centerPos.y - halfY ), Vec2( centerPos.x + halfX, centerPos.y + halfY ) );
	m_button = new UIButton2D( centerPos, sizeX, sizeY, "", "BuyItem", Rgba8( 30, 30, 50 ), Rgba8( 60, 60, 100 ) );
	m_button->AddArg( "ItemType", item->m_type ); 
	m_button->AddArg( "ItemName", item->m_name );

	m_button->AddArg( "m_healing", std::to_string(item->m_statBonuses.m_healing) );
	m_button->AddArg( "m_rangeBoost", std::to_string(item->m_statBonuses.m_rangeBoost) );
	m_button->AddArg( "m_meleeBoost", std::to_string(item->m_statBonuses.m_meleeBoost) );
	m_button->AddArg( "m_armor", std::to_string(item->m_statBonuses.m_armor) );
	m_button->AddArg( "m_magicResistance", std::to_string(item->m_statBonuses.m_magicResistance) );
	m_button->AddArg( "m_lifeSteal", std::to_string(item->m_statBonuses.m_lifeSteal) );
	m_button->AddArg( "m_dodge", std::to_string(item->m_statBonuses.m_dodge) );
	m_button->AddArg( "m_speedBoost", std::to_string(item->m_statBonuses.m_speedBoost) );

	m_button->m_defaultTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/UI/png@4x/Button/Rect/Default@4x.png" );
	m_button->m_hoveredTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/UI/png@4x/Button/Rect/Hover@4x.png" );
	if ( !item->m_iconTexture.empty() )
	{
		m_icon = g_engine->m_render->CreateOrGetTextureFromFile( item->m_iconTexture.c_str() );
	}
	m_font = g_engine->m_render->CreateOrGetBitmapFont( "Data/Fonts/SquirrelFixedFont" );
	BuildStatLines();
}

//------------------------------------------------------------------------------
ShopItemCard::~ShopItemCard()
{
	delete m_button;
}

//------------------------------------------------------------------------------
void ShopItemCard::Update( const Vec2& mousePos, bool isMouseLeftJustPressed )
{
	m_button->Update( mousePos, isMouseLeftJustPressed );
}

//------------------------------------------------------------------------------
void ShopItemCard::Render() const
{
	m_button->Render();

	float cardWidth = m_bounds.m_maxs.x - m_bounds.m_mins.x;
	float cardHeight = m_bounds.m_maxs.y - m_bounds.m_mins.y;
	float headerH = cardHeight * 0.25f;
	float footerH = cardHeight * 0.2f;
	float iconSize = cardWidth * 0.3f;
	float padding = 6.f;

	// Header (top)
	AABB2 headerBounds( Vec2( m_bounds.m_mins.x, m_bounds.m_maxs.y - headerH ), m_bounds.m_maxs );

	// icon in header
	// maybe move it down 
	AABB2 iconBounds( Vec2( headerBounds.m_mins.x + padding, headerBounds.m_mins.y + padding ), Vec2( headerBounds.m_mins.x + iconSize - padding, headerBounds.m_maxs.y - padding ) );
	if ( m_icon )
	{
		std::vector<Vertex> iconVerts;
		AddVertsForAABB2D( iconVerts, iconBounds, Rgba8::WHITE );
		g_engine->m_render->BindTexture( m_icon );
		g_engine->m_render->DrawVertexArray( iconVerts );
	}

	// item name and type
	float nameX = headerBounds.m_mins.x + iconSize + padding;
	AABB2 nameBounds( Vec2( nameX, headerBounds.m_mins.y + headerH * 0.45f ), Vec2( headerBounds.m_maxs.x - padding, headerBounds.m_maxs.y - padding ) );
	AABB2 typeBounds( Vec2( nameX, headerBounds.m_mins.y + padding ), Vec2( headerBounds.m_maxs.x - padding, headerBounds.m_mins.y + headerH * 0.45f ) );
	std::vector<Vertex> textVerts;
	m_font->AddVertsForTextInBox2D( textVerts, m_item->m_name, nameBounds, 14.f, Rgba8::WHITE, 0.7f, Vec2( 0.f, 0.5f ) );
	m_font->AddVertsForTextInBox2D( textVerts, m_item->m_type, typeBounds, 11.f, Rgba8( 180, 180, 180 ), 0.7f, Vec2( 0.f, 0.5f ) );

	// Stats (middle region)
	AABB2 statsBounds( Vec2( m_bounds.m_mins.x, m_bounds.m_mins.y + footerH ), Vec2( m_bounds.m_maxs.x, m_bounds.m_maxs.y - headerH ) );
	float lineHeight = 13.f;
	float statY = statsBounds.m_maxs.y - lineHeight - padding;

	for ( const std::string& line : m_statLines )
	{
		AABB2 lineBounds( Vec2( statsBounds.m_mins.x + padding, statY ), Vec2( statsBounds.m_maxs.x - padding, statY + lineHeight ) );
		m_font->AddVertsForTextInBox2D( textVerts, line, lineBounds, lineHeight, Rgba8( 100, 220, 100 ), 0.7f, Vec2( 0.f, 0.5f ) );
		statY -= lineHeight + 3.f;
	}

	// cost (bottom)
	AABB2 costBounds( m_bounds.m_mins, Vec2( m_bounds.m_maxs.x, m_bounds.m_mins.y + footerH ) );
	m_font->AddVertsForTextInBox2D( textVerts, Stringf( "%d Gold", m_item->m_cost ), costBounds, 14.f, Rgba8( 255, 215, 0 ), 0.7f, Vec2( 0.5f, 0.5f ) );

	g_engine->m_render->BindTexture( &m_font->GetTexture() );
	g_engine->m_render->DrawVertexArray( textVerts );
}

//------------------------------------------------------------------------------
void ShopItemCard::BuildStatLines()
{
	const ItemStatBonuses& s = m_item->m_statBonuses;
	if ( s.m_healing != 0 ) m_statLines.push_back( Stringf( "+%d Healing", s.m_healing ) ); // #todo need to implement check for when items are decrease
	if ( s.m_armor != 0 ) m_statLines.push_back( Stringf( "+%d Armor", s.m_armor ) );
	if ( s.m_magicResistance != 0 ) m_statLines.push_back( Stringf( "+%d Magic Resistance", s.m_magicResistance ) );
	if ( s.m_rangeBoost != 0.f ) m_statLines.push_back( Stringf( "+%.0f%% Range Boost", s.m_rangeBoost * 100.f ) );
	if ( s.m_meleeBoost != 0.f ) m_statLines.push_back( Stringf( "+%.0f%% Melee Boost", s.m_meleeBoost * 100.f ) );
	if ( s.m_lifeSteal != 0.f ) m_statLines.push_back( Stringf( "+%.0f%% Life Steal", s.m_lifeSteal * 100.f ) );
	if ( s.m_dodge != 0.f ) m_statLines.push_back( Stringf( "+%.0f%% Dodge", s.m_dodge * 100.f ) );
	if ( s.m_speedBoost != 0.f ) m_statLines.push_back( Stringf( "+%.0f%% Speed Boost", s.m_speedBoost * 100.f ) );
}