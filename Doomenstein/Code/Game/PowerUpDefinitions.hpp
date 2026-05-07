#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/FloatRange.hpp"
#include <map>
#include <string>

//-----------------------------------------------------------------------------------------------
enum class PowerUpType
{
	SPEED_BOOST,
	SHOOT_SPEED,
	HEALTH_BOOST,
	INVALID
};

//-----------------------------------------------------------------------------------------------
class PowerUpDefinition
{
public:
	PowerUpDefinition() = default;
	~PowerUpDefinition() = default;

	static void InitializePowerUpDefs();
	static const PowerUpDefinition* GetByName( const std::string& name );
	static PowerUpType GetTypeFromName( const std::string& name );

public:
	std::string m_name;
	PowerUpType m_type = PowerUpType::INVALID;
	FloatRange m_percentage = FloatRange( 1.0f, 10.0f );

	static std::map<std::string, PowerUpDefinition> s_definitions;
};