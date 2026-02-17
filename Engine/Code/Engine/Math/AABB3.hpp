#pragma once
#include "Engine/Math/Vec3.hpp"

//------------------------------------------------------------------------------

struct AABB3
{
public:
	Vec3 m_mins;
	Vec3 m_maxs;

public:
	// Construction/Destruction
	~AABB3() {}                                          // destructor (do nothing)
	AABB3() {}                                           // default constructor (do nothing)
	AABB3( AABB3 const& copyFrom );                      // copy constructor (from another Vec2)
	explicit AABB3( float minX, float minY, float minZ, float maxX, float maxY, float maxZ );  // explicit constructor (from x1,y1, x2,y2)
	explicit AABB3( Vec3 const& mins, Vec3 const& maxs );   // explicit constructor (from mins, maxs)
};