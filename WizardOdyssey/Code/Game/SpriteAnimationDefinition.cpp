#include "Game/SpriteAnimationDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"

std::map<std::string, SpriteAnimationDefinition> SpriteAnimationDefinition::s_definitions;

//-----------------------------------------------------------------------------------------------
static void LoadSpriteAnimationDefsFromFile( char const* filePath )
{
	XmlDocument doc;
	doc.LoadFile( filePath );

	XmlElement* root = doc.RootElement();
	if ( !root )
	{
		return;
	}

	XmlElement* actorElem = root->FirstChildElement( "ActorDefinition" );
	XmlUtils xml;

	while ( actorElem )
	{
		std::string actorName = xml.ParseXmlAttribute( *actorElem, "name", "" );

		XmlElement* visualsElem = actorElem->FirstChildElement( "Visuals" );
		if ( visualsElem )
		{
			SpriteAnimationDefinition def;
			def.m_name = actorName;
			def.m_spriteSheetPath = xml.ParseXmlAttribute( *visualsElem, "spriteSheet", "" );
			def.m_cellCount = xml.ParseXmlAttribute( *visualsElem, "cellCount", IntVec2( 0, 0 ) );
			std::string billboardTypeStr = xml.ParseXmlAttribute( *visualsElem, "billboardType", "WorldUpFacing" );
			def.m_billboardType = StringToBillboardType( billboardTypeStr );
			def.m_size = xml.ParseXmlAttribute( *visualsElem, "size", Vec2( 1.0f, 1.0f ) );
			def.m_pivot = xml.ParseXmlAttribute( *visualsElem, "pivot", Vec2( 0.5f, 0.5f ) );

			XmlElement* animGroupElem = visualsElem->FirstChildElement( "AnimationGroup" );
			while ( animGroupElem )
			{
				SpriteAnimationGroupDefinition animGroup;
				animGroup.m_name = xml.ParseXmlAttribute( *animGroupElem, "name", "" );
				animGroup.m_secondsPerFrame = xml.ParseXmlAttribute( *animGroupElem, "secondsPerFrame", 0.1f );
				animGroup.m_scaleBySpeed = xml.ParseXmlAttribute( *animGroupElem, "scaleBySpeed", false );
				std::string playbackMode = xml.ParseXmlAttribute( *animGroupElem, "playbackMode", "Loop" );
				animGroup.m_playbackMode = ( playbackMode == "Once" ) ? SpriteAnimPlaybackType::ONCE : SpriteAnimPlaybackType::LOOP;

				XmlElement* directionElem = animGroupElem->FirstChildElement( "Direction" );
				while ( directionElem )
				{
					Vec3 direction = xml.ParseXmlAttribute( *directionElem, "vector", Vec3::ZERO );

					XmlElement* animElem = directionElem->FirstChildElement( "Animation" );
					if ( animElem )
					{
						int startFrame = xml.ParseXmlAttribute( *animElem, "startFrame", 0 );
						int endFrame = xml.ParseXmlAttribute( *animElem, "endFrame", 0 );
						animGroup.m_directionalAnims.push_back( { direction, startFrame, endFrame } );
					}

					directionElem = directionElem->NextSiblingElement( "Direction" );
				}

				def.m_animationGroups.push_back( animGroup );
				animGroupElem = animGroupElem->NextSiblingElement( "AnimationGroup" );
			}

			SpriteAnimationDefinition::s_definitions[actorName] = def;
		}

		actorElem = actorElem->NextSiblingElement( "ActorDefinition" );
	}
}

//-----------------------------------------------------------------------------------------------
void SpriteAnimationDefinition::InitializeSpriteAnimationDefs()
{
	LoadSpriteAnimationDefsFromFile( "Data/Definitions/ActorDefinitions.xml" );
	LoadSpriteAnimationDefsFromFile( "Data/Definitions/ProjectileActorDefinitions.xml" );
}

//-----------------------------------------------------------------------------------------------
const SpriteAnimationDefinition* SpriteAnimationDefinition::GetByName( const std::string& name )
{
	std::map<std::string, SpriteAnimationDefinition>::iterator it = s_definitions.find( name );
	if ( it == s_definitions.end() )
	{
		return nullptr;
	}

	const SpriteAnimationDefinition& foundDefinition = it->second;
	return &foundDefinition;
}

//-----------------------------------------------------------------------------------------------
BillboardType StringToBillboardType( const std::string& typeStr )
{
	auto it = s_billboardTypeMap.find( typeStr );
	if ( it != s_billboardTypeMap.end() )
	{
		return it->second;
	}
	return BillboardType::FULL_FACING;
}