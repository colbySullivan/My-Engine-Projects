#pragma once
#include "Vec2.hpp"
#include "Vec3.hpp"

class RandomNumberGenerator
{
public:
    int     RollRandomIntLessThan(int maxNotInclusive);
    int     RollRandomIntInRange(int minInclusive, int maxInclusive);
    float   RollRandomFloatZeroToOne();
    float   RollRandomFloatInRange(float minInclusive, float maxInclusive);
    Vec2    GetRandom2DPosition( float minX, float maxX, float minY, float maxY );
    Vec3    GetRandomVectorInCone( const Vec3& direction, float coneDegrees );

private:
    // unsigned int         m_seed = 0;
    // int                  m_position = 0;
};