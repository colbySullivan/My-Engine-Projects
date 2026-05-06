#include "Game/WeaponDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/IntVec2.hpp"

//-----------------------------------------------------------------------------------------------
std::map<std::string, WeaponDefinition> WeaponDefinition::s_definitions;

//-----------------------------------------------------------------------------------------------
void WeaponDefinition::InitializeWeaponDefs()
{
	XmlDocument doc;
	doc.LoadFile( "Data/Definitions/WeaponDefinitions.xml" );

	XmlElement* root = doc.RootElement();
	if ( !root )
	{
		return;
	}

	XmlElement* weaponElem = root->FirstChildElement( "WeaponDefinition" );
	XmlUtils xml;

	while ( weaponElem )
	{
		WeaponDefinition def;

		def.m_name       = xml.ParseXmlAttribute( *weaponElem, "name",       std::string( "Unnamed" ) );
		def.m_refireTime = xml.ParseXmlAttribute( *weaponElem, "refireTime", 0.f );

		def.m_rayCount   = xml.ParseXmlAttribute( *weaponElem, "rayCount",   0 );
		def.m_rayCone    = xml.ParseXmlAttribute( *weaponElem, "rayCone",    0.f );
		def.m_rayRange   = xml.ParseXmlAttribute( *weaponElem, "rayRange",   0.f );
		def.m_rayDamage  = xml.ParseXmlAttribute( *weaponElem, "rayDamage",  FloatRange( 0.f, 1.f ), '~' );
		def.m_rayImpulse = xml.ParseXmlAttribute( *weaponElem, "rayImpulse", 0.f );

		def.m_projectileCount       = xml.ParseXmlAttribute( *weaponElem, "projectileCount", 0 );
		def.m_projectileActorName   = xml.ParseXmlAttribute( *weaponElem, "projectileActor", std::string( "" ) );
		def.m_projectileConeDegrees = xml.ParseXmlAttribute( *weaponElem, "projectileCone",  0.f );
		def.m_projectileSpeed       = xml.ParseXmlAttribute( *weaponElem, "projectileSpeed", 0.f );

		def.m_meleeCount    = xml.ParseXmlAttribute( *weaponElem, "meleeCount",  0 );
		def.m_meleeArcDegrees = xml.ParseXmlAttribute( *weaponElem, "meleeArc",   0.f );
		def.m_meleeRange    = xml.ParseXmlAttribute( *weaponElem, "meleeRange",  0.f );
		def.m_meleeDamage   = xml.ParseXmlAttribute( *weaponElem, "meleeDamage", FloatRange( 0.f, 1.f ), '~');
		def.m_meleeImpulse  = xml.ParseXmlAttribute( *weaponElem, "meleeImpulse", 0.f );

		XmlElement* hudElem = weaponElem->FirstChildElement( "HUD" );
		if ( hudElem )
		{
			def.m_hud.m_shader = xml.ParseXmlAttribute( *hudElem, "shader", std::string( "Default" ) );
			def.m_hud.m_baseTexture = xml.ParseXmlAttribute( *hudElem, "baseTexture", std::string( "" ) );
			def.m_hud.m_reticleTexture = xml.ParseXmlAttribute( *hudElem, "reticleTexture", std::string( "" ) );
			def.m_hud.m_reticleSize = xml.ParseXmlAttribute( *hudElem, "reticleSize", Vec2( 0.f, 0.f ) );
			def.m_hud.m_spriteSize = xml.ParseXmlAttribute( *hudElem, "spriteSize", Vec2( 0.f, 0.f ) );
			def.m_hud.m_spritePivot = xml.ParseXmlAttribute( *hudElem, "spritePivot", Vec2( 0.5f, 0.5f ) );

			XmlElement* animElem = hudElem->FirstChildElement( "Animation" );
			while ( animElem )
			{
				WeaponAnimationDefinition animDef;
				animDef.m_name = xml.ParseXmlAttribute( *animElem, "name", std::string( "Unnamed" ) );
				animDef.m_shader = xml.ParseXmlAttribute( *animElem, "shader", std::string( "Default" ) );
				animDef.m_spriteSheet = xml.ParseXmlAttribute( *animElem, "spriteSheet", std::string( "" ) );
				animDef.m_cellCount = xml.ParseXmlAttribute( *animElem, "cellCount", IntVec2( 1, 1 ) );
				animDef.m_secondsPerFrame = xml.ParseXmlAttribute( *animElem, "secondsPerFrame", 0.1f );
				animDef.m_startFrame = xml.ParseXmlAttribute( *animElem, "startFrame", 0 );
				animDef.m_endFrame = xml.ParseXmlAttribute( *animElem, "endFrame", 0 );

				def.m_hud.m_animations.push_back( animDef );
				animElem = animElem->NextSiblingElement( "Animation" );
			}
		}

		XmlElement* soundsElem = weaponElem->FirstChildElement( "Sounds" );
		if ( soundsElem )
		{
			XmlElement* soundElem = soundsElem->FirstChildElement( "Sound" );
			while ( soundElem )
			{
				WeaponSoundDefinition soundDef;
				soundDef.m_soundName = xml.ParseXmlAttribute( *soundElem, "sound", std::string( "" ) );
				soundDef.m_filePath = xml.ParseXmlAttribute( *soundElem, "name", std::string( "" ) );
				soundDef.m_volume = xml.ParseXmlAttribute( *soundElem, "soundLevel", 1.f );

				def.m_sounds.push_back( soundDef );
				soundElem = soundElem->NextSiblingElement( "Sound" );
			}
		}

		s_definitions[def.m_name] = def;

		weaponElem = weaponElem->NextSiblingElement( "WeaponDefinition" );
	}
}

//-----------------------------------------------------------------------------------------------
const WeaponDefinition* WeaponDefinition::GetByName( const std::string& name )
{
	std::map<std::string, WeaponDefinition>::iterator it = s_definitions.find( name );
	if ( it == s_definitions.end() )
	{
		return nullptr;
	}

	const WeaponDefinition& foundDefinition = it->second;
	return &foundDefinition;
}

//-----------------------------------------------------------------------------------------------
const WeaponAnimationDefinition* WeaponDefinition::GetAnimationByName( const std::string& name ) const
{
	for ( const WeaponAnimationDefinition& anim : m_hud.m_animations )
	{
		if ( anim.m_name == name )
		{
			return &anim;
		}
	}
	return nullptr;
}

//------------------------------------------------------------------------------
const WeaponSoundDefinition* WeaponDefinition::GetSoundByName( const std::string& name ) const
{
	for ( const WeaponSoundDefinition& soundDef : m_sounds )
	{
		if ( soundDef.m_soundName == name )
		{
			return &soundDef;
		}
	}

	return nullptr;
}
