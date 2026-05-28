#include "RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Vec2.hpp"
#include "EulerAngles.hpp"
#include <stdlib.h>
#include <cmath>

int RandomNumberGenerator::RollRandomIntLessThan(int maxNotInclusive)
{
	return  rand() % maxNotInclusive;
}

//-----------------------------------------------------------------------------------------------
int RandomNumberGenerator::RollRandomIntInRange(int minInclusive, int maxInclusive)
{
	return  rand() % ( maxInclusive - minInclusive + 1 ) + minInclusive;
}

//-----------------------------------------------------------------------------------------------
float RandomNumberGenerator::RollRandomFloatZeroToOne()
{
	return (float)rand() / (float)RAND_MAX;
}

//-----------------------------------------------------------------------------------------------
float RandomNumberGenerator::RollRandomFloatInRange(float minInclusive, float maxInclusive)
{
	return RollRandomFloatZeroToOne() * (maxInclusive - minInclusive) + minInclusive;
}

//-----------------------------------------------------------------------------------------------
Vec2 RandomNumberGenerator::GetRandom2DPosition( float minX, float maxX, float minY, float maxY )
{
	return Vec2( RollRandomFloatInRange( minX, maxX ), RollRandomFloatInRange( minY, maxY ) );
}

//-----------------------------------------------------------------------------------------------
Vec3 RandomNumberGenerator::GetRandomVectorInCone( const Vec3& direction, float coneDegrees )
{
	Vec3 normalizedDir = direction.GetNormalized();
	float yaw = Atan2Degrees( normalizedDir.y, normalizedDir.x );
	float pitch = Atan2Degrees( -normalizedDir.z, sqrtf( normalizedDir.x * normalizedDir.x + normalizedDir.y * normalizedDir.y ) );

	float halfCone = coneDegrees * 0.5f;
	float randomYawOffset = RollRandomFloatInRange( -halfCone, halfCone );
	float randomPitchOffset = RollRandomFloatInRange( -halfCone, halfCone );

	EulerAngles randomAngles( yaw + randomYawOffset, pitch + randomPitchOffset, 0.f );

	return randomAngles.GetForwardDir_IFwd_JLeft_KUp();
}