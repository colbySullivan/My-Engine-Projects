#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <map>
#include <string>
#include <vector>
#include "Engine/Math/MathUtils.hpp"

enum SpriteAnimPlaybackMode
{
	LOOP,
	ONCE,
	numOfPlaybackModes
};

struct DirectionalAnimInfo
{
	Vec3 direction;
	int startFrame;
	int endFrame;
};

struct SpriteAnimationGroupDefinition
{
	std::string m_name;
	float m_secondsPerFrame;
	bool m_scaleBySpeed;
	SpriteAnimPlaybackMode m_playbackMode;
	std::vector<DirectionalAnimInfo> m_directionalAnims;
};

static std::map<std::string, BillboardType> s_billboardTypeMap =
{
	{ "FullFacing", BillboardType::FULL_FACING },
	{ "FullOpposing", BillboardType::FULL_OPPOSING },
	{ "WorldUpFacing", BillboardType::WORLD_UP_FACING },
	{ "WorldUpOpposing", BillboardType::WORLD_UP_OPPOSING }
};

BillboardType StringToBillboardType( const std::string& typeStr );

class SpriteAnimationDefinition
{
public:
	SpriteAnimationDefinition() = default;
	static void InitializeSpriteAnimationDefs();
	static const SpriteAnimationDefinition* GetByName( const std::string& name );

	static std::map<std::string, SpriteAnimationDefinition> s_definitions;
	std::string m_name;
	std::string m_spriteSheetPath;
	IntVec2 m_cellCount;
	std::vector<SpriteAnimationGroupDefinition> m_animationGroups;
	BillboardType m_billboardType;
	Vec2 m_size;
	Vec2 m_pivot;
};