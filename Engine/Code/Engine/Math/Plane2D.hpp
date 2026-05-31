#pragma once
#include "Engine/Math/Vec2.hpp"

struct Plane2D
{
public:
	Vec2 m_normal;
	float m_dist;

public:
	Plane2D() {}
	~Plane2D() {}
	explicit Plane2D( Vec2 normal, Vec2 point );

	Vec2 GetNormal();
	float GetPlaneDistance();
	float GetAltitude( Vec2 point );
	bool isFrontSide( Vec2 point );
	bool isBackSide( Vec2 point );
	bool isOnPlane( Vec2 point );
};