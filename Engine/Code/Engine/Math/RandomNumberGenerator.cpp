#include "RandomNumberGenerator.hpp"
#include "Vec2.hpp"
#include <stdlib.h>

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
