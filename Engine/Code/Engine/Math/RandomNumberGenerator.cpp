#include "RandomNumberGenerator.hpp"
#include <stdlib.h>

int RandomNumberGenerator::RollRandomIntLessThan(int maxNotInclusive)
{
	return  rand() % maxNotInclusive;
}

int RandomNumberGenerator::RollRandomIntInRange(int minInclusive, int maxInclusive)
{
	return  rand() % ( maxInclusive - minInclusive + 1 ) + minInclusive;
}

float RandomNumberGenerator::RollRandomFloatZeroToOne()
{
	return (float)rand() / RAND_MAX;
}

float RandomNumberGenerator::RollRandomFloatInRange(float minInclusive, float maxInclusive)
{
	return RollRandomFloatZeroToOne() * (maxInclusive - minInclusive) + minInclusive;
}
