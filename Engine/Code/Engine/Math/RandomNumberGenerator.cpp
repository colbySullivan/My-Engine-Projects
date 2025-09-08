#include "RandomNumberGenerator.hpp"
#include <stdlib.h>

int RandomNumberGenerator::RollRandomIntLessThan(int maxNotInclusive)
{
	int randInRangeLessThan = rand() % maxNotInclusive;
	return randInRangeLessThan;
}

int RandomNumberGenerator::RollRandomIntInRange(int minInclusive, int maxInclusive)
{
	int range = maxInclusive - minInclusive + 1;
	int randIntInBetween = rand() % range + minInclusive;
	return randIntInBetween;
}

float RandomNumberGenerator::RollRandomFloatZeroToOne()
{
	float randFloatZeroToOne = (float)rand() / RAND_MAX;
	return randFloatZeroToOne;
}

float RandomNumberGenerator::RollRandomFloatInRange(float minInclusive, float maxInclusive)
{
	float randFloatInRangeBetween = RollRandomFloatZeroToOne() * (maxInclusive - minInclusive) + minInclusive;
	return randFloatInRangeBetween;
}
