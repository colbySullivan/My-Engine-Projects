#include "Game/ActorDefinitions.hpp"
#include "Game/SpriteAnimationDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"

std::map<std::string, ActorDefinitions> ActorDefinitions::s_definitions;

//-----------------------------------------------------------------------------------------------
void ActorDefinitions::InitializeActorDefs()
{
	LoadActorDefsFromFile( "Data/Definitions/ActorDefinitions.xml" );
}

//-----------------------------------------------------------------------------------------------
void ActorDefinitions::LoadActorDefsFromFile( char const* filePath )
{
	XmlDocument doc;
	doc.LoadFile( filePath );

	XmlElement* root = doc.RootElement();
	if ( !root )
		return;

	XmlUtils xml;
	XmlElement* actorElem = root->FirstChildElement( "ActorDefinition" );

	while ( actorElem )
	{
		ActorDefinitions def;

		def.m_name = xml.ParseXmlAttribute( *actorElem, "name", "" );
		def.m_health = xml.ParseXmlAttribute( *actorElem, "health", 100 );

		XmlElement* collisionElem = actorElem->FirstChildElement( "Collision" );
		if ( collisionElem )
		{
			def.m_collision.m_physicRadius = xml.ParseXmlAttribute( *collisionElem, "physicRadius", 0.f );
			def.m_collision.m_cosmeticRadius = xml.ParseXmlAttribute( *collisionElem, "cosmeticRadius", 0.f );
		}

		XmlElement* physicsElem = actorElem->FirstChildElement( "Physics" );
		if ( physicsElem )
		{
			def.m_physics.m_walkSpeed = xml.ParseXmlAttribute( *physicsElem, "walkSpeed", 1.5f );
			def.m_physics.m_runSpeed = xml.ParseXmlAttribute( *physicsElem, "runSpeed", 6.0f );
		}

		XmlElement* visualsElem = actorElem->FirstChildElement( "Visuals" );
		if ( visualsElem )
		{
			def.m_visuals.m_visualSize = xml.ParseXmlAttribute( *visualsElem, "size", Vec2( 1.f, 1.f ) );
		}

		XmlElement* soundsElem = actorElem->FirstChildElement( "Sounds" );
		if ( soundsElem )
		{
			XmlElement* soundChild = soundsElem->FirstChildElement( "Sound" );
			while ( soundChild )
			{
				std::string soundType = xml.ParseXmlAttribute( *soundChild, "sound", "" );
				std::string soundPath = xml.ParseXmlAttribute( *soundChild, "name", "" );

				if ( soundType == "Hurt" )       def.m_sounds.m_hurtSoundPath = soundPath;
				else if ( soundType == "Death" ) def.m_sounds.m_deathSoundPath = soundPath;

				soundChild = soundChild->NextSiblingElement( "Sound" );
			}
		}

		s_definitions[def.m_name] = def;

		actorElem = actorElem->NextSiblingElement( "ActorDefinition" );
	}
}

//-----------------------------------------------------------------------------------------------
const ActorDefinitions* ActorDefinitions::GetByName( const std::string& name )
{
	auto it = s_definitions.find( name );
	if ( it == s_definitions.end() )
	{
		return nullptr;
	}
	return &( it->second );
}