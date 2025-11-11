#pragma once
#include "Engine/Renderer/SpriteSheet.hpp"


//----------------------------------------------------------------------------------------
class SpriteDefinition
{
public:
	void                GetUVs( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs ) const;
	AABB2               GetUVs() const;
	SpriteSheet const&	GetSpriteSheet() const;
	Texture&			GetTexture() const;
	float               GetAspect() const;
	SpriteSheet const*	m_spriteSheet;
	int                 m_spriteIndex = -1;
	Vec2                m_uvAtMins = Vec2(0.f,0.f);
	Vec2                m_uvAtMaxs = Vec2(1.f,1.f);
};