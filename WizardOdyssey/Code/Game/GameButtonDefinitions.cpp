#include "Game/GameButtonDefinitions.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Engine.hpp"

std::vector<GameButtonDefinitions> GameButtonDefinitions::s_definitions;

//-----------------------------------------------------------------------------------------------
void GameButtonDefinitions::InitializeButtonDefs()
{
	LoadButtonDefsFromFile( "Data/Definitions/GameButtons.xml" );
}

//-----------------------------------------------------------------------------------------------
void GameButtonDefinitions::LoadButtonDefsFromFile( char const* filePath )
{
	XmlDocument doc;
	doc.LoadFile( filePath );

	XmlElement* root = doc.RootElement();
	if ( !root )
		return;

	XmlUtils xml;
	XmlElement* buttonElem = root->FirstChildElement( "ButtonDefinition" );

	while ( buttonElem )
	{
		GameButtonDefinitions def;

		def.name = xml.ParseXmlAttribute( *buttonElem, "name", "" );
		def.positionFraction = xml.ParseXmlAttribute( *buttonElem, "positionFraction", Vec2(0.f,0.f) );
		def.sizeX = xml.ParseXmlAttribute( *buttonElem, "sizeX", 1.f );
		def.sizeY = xml.ParseXmlAttribute( *buttonElem, "sizeY", 1.f );
		def.buttonText = xml.ParseXmlAttribute( *buttonElem, "buttonText", "" );
		def.onClickEvent = xml.ParseXmlAttribute( *buttonElem, "onClickEvent", "" );
		def.baseColor = xml.ParseXmlAttribute( *buttonElem, "baseColor", Rgba8::WHITE );
		def.hoveredColor = xml.ParseXmlAttribute( *buttonElem, "hoveredColor", Rgba8::WHITE );
		def.argName = xml.ParseXmlAttribute( *buttonElem, "argName", "" );
		def.argValue = xml.ParseXmlAttribute( *buttonElem, "argValue", "" );
		def.gameState = xml.ParseXmlAttribute( *buttonElem, "gameState", 0 );
		def.defaultTexture = xml.ParseXmlAttribute( *buttonElem, "defaultTexture", "" );
		def.hoveredTexture = xml.ParseXmlAttribute( *buttonElem, "hoveredTexture", "" );

		// Set button position based on xy fraction
		float screenSizeY = g_gameConfig->GetValue( "screenSizeY", 800.f );
		float screenSizeX = g_gameConfig->GetValue( "screenSizeX", 1600.f );
		Vec2 screenPos = Vec2( def.positionFraction.x * screenSizeX, def.positionFraction.y * screenSizeY );

		UIButton2D* button = new UIButton2D( screenPos, def.sizeX, def.sizeY, def.buttonText, def.onClickEvent, def.baseColor, def.hoveredColor);
		if ( !def.argName.empty() && !def.argValue.empty() )
		{
			button->AddArg( def.argName, def.argValue );
		}

		if ( !def.defaultTexture.empty() )
		{
			button->m_defaultTexture = g_engine->m_render->CreateOrGetTextureFromFile( def.defaultTexture.c_str() );
		}

		if ( !def.hoveredTexture.empty() )
		{
			button->m_hoveredTexture = g_engine->m_render->CreateOrGetTextureFromFile( def.hoveredTexture.c_str() );
		}

		def.buttonRef = button;

		s_definitions.push_back( def );

		buttonElem = buttonElem->NextSiblingElement( "ButtonDefinition" );
	}
}