#pragma once
#include "Engine/Math/Vec3.hpp"

struct Plane3D
{
public:
	Vec3 m_normal;
	float m_dist;
public: 
	Plane3D() {};
	~Plane3D() {};
	explicit Plane3D( Vec3 normal, Vec3 centerPoint );

	Vec3 GetNormal();
	float GetPlaneDistance();
	float GetAltitude( Vec3 point );
	bool isFrontSide( Vec3 point );
	bool isBackSide( Vec3 point );
	bool isPointOnPlane( Vec3 point );

};