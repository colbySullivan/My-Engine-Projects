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
	DAMAGE_BOOST,
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

	static std::string GetRandomPowerUp();

public:
	std::string m_name;
	PowerUpType m_type = PowerUpType::INVALID;
	FloatRange m_percentage = FloatRange( 1.0f, 10.0f );
	std::string m_effectDescription;
	std::string m_imagePath;
	float m_weight = 0.f;


	static std::map<std::string, PowerUpDefinition> s_definitions;
};