#include "Game/ActorDefinitions.hpp"
#include "Engine/Core/XmlUtils.hpp"

std::map<std::string, ActorDefinition> ActorDefinition::s_definitions;

void ActorDefinition::InitializeActorDefs()
{
	XmlDocument doc;
	doc.LoadFile( "Data/Definitions/ActorDefinitions.xml" );


	XmlElement* root = doc.RootElement();
	XmlElement* actorElem = root->FirstChildElement( "ActorDefinition" );

	while ( actorElem )
	{
		ActorDefinition def;
		XmlUtils xml;

		def.m_name = xml.ParseXmlAttribute( *actorElem, "name", "Unnamed" );
		def.m_visible = xml.ParseXmlAttribute( *actorElem, "visible", false );
		def.m_health = xml.ParseXmlAttribute( *actorElem, "health", 1 );
		def.m_corpseLifetime = xml.ParseXmlAttribute( *actorElem, "corpseLifetime", 0.f );
		def.m_faction = xml.ParseXmlAttribute( *actorElem, "faction", "NEUTRAL" );
		def.m_canBePossessed = xml.ParseXmlAttribute( *actorElem, "canBePossessed", false );

		XmlElement* collElem = actorElem->FirstChildElement( "Collision" );
		if ( collElem ) 
		{
			def.m_physicsRadius = xml.ParseXmlAttribute( *collElem, "physicsRadius", 0.f );
			def.m_physicsHeight = xml.ParseXmlAttribute( *collElem, "physicsHeight", 0.f );
			def.m_collidesWithWorld = xml.ParseXmlAttribute( *collElem, "collidesWithWorld", false );
			def.m_collidesWithActors = xml.ParseXmlAttribute( *collElem, "collidesWithActors", false );
		}

		XmlElement* physElem = actorElem->FirstChildElement( "Physics" );
		if ( physElem ) {
			def.m_simulated = xml.ParseXmlAttribute( *physElem, "simulated", false );
			def.m_walkSpeed = xml.ParseXmlAttribute( *physElem, "walkSpeed", 0.f );
			def.m_runSpeed = xml.ParseXmlAttribute( *physElem, "runSpeed", 0.f );
			def.m_drag = xml.ParseXmlAttribute( *physElem, "drag", 0.f );
			def.m_turnSpeed = xml.ParseXmlAttribute( *physElem, "turnSpeed", 0.f );
		}

		XmlElement* aiElem = actorElem->FirstChildElement( "AI" );
		if ( aiElem ) {
			def.m_aiEnabled = xml.ParseXmlAttribute( *aiElem, "aiEnabled", false );
		}

		XmlElement* inventoryElem = actorElem->FirstChildElement( "Inventory" );
		if ( inventoryElem )
		{
			XmlElement* weaponElem = inventoryElem->FirstChildElement( "Weapon" );
			while ( weaponElem )
			{
				std::string weaponName = xml.ParseXmlAttribute( *weaponElem, "name", "" );
				if ( !weaponName.empty() )
				{
					def.m_weaponNames.push_back( weaponName );
				}
				weaponElem = weaponElem->NextSiblingElement( "Weapon" );
			}
		}

		XmlElement* visualsElem = actorElem->FirstChildElement( "Visuals" );
		if ( visualsElem )
		{
			def.m_visualSize = xml.ParseXmlAttribute( *visualsElem, "size", Vec2( 1.f, 1.f ) );
			def.m_visualPivot = xml.ParseXmlAttribute( *visualsElem, "pivot", Vec2( 0.5f, 0.f ) );
			def.m_billboardType = xml.ParseXmlAttribute( *visualsElem, "billboardType", std::string( "WorldUpFacing" ) );
			def.m_renderLit = xml.ParseXmlAttribute( *visualsElem, "renderLit", false );
			def.m_renderRounded = xml.ParseXmlAttribute( *visualsElem, "renderRounded", false );
			def.m_shaderPath = xml.ParseXmlAttribute( *visualsElem, "shader", std::string( "" ) );
			def.m_spriteSheetPath = xml.ParseXmlAttribute( *visualsElem, "spriteSheet", std::string( "" ) );
			def.m_cellCount = xml.ParseXmlAttribute( *visualsElem, "cellCount", IntVec2( 1, 1 ) );
		}

		s_definitions[def.m_name] = def;
		actorElem = actorElem->NextSiblingElement( "ActorDefinition" );
	}
}

const ActorDefinition* ActorDefinition::GetByName( const std::string& name )
{
	std::map<std::string, ActorDefinition>::iterator it = s_definitions.find( name );
	if ( it == s_definitions.end() )
	{
		return nullptr;
	}

	const ActorDefinition& foundDefinition = it->second;
	return &foundDefinition;
}