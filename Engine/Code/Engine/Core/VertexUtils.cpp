#include "VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"



void TransformVertexArrayXY3D( int numVerts, Vertex* verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY )
{
	Vec2 iBasis = Vec2::MakeFromPolarDegrees( rotationDegreesAboutZ, uniformScaleXY );
	Vec2 jBasis = iBasis.GetRotatedBy90Degrees();
	for ( int vertIndex = 0; vertIndex < numVerts; ++vertIndex )
	{
		Vec3& pos = verts[vertIndex].m_position;
		//TransformPositionXY3D(pos, uniformScaleXY, rotationDegreesAboutZ, translationXY); // Old version
		TransformPositionXY3D( pos, iBasis, jBasis, translationXY );
	}
}

//------------------------------------------------------------------------------
void AddVertsForDisc2D( [[maybe_unused]] std::vector<Vertex>& verts, [[maybe_unused]] Vec2 const& center, [[maybe_unused]] float discRadius, [[maybe_unused]] Rgba8 color)
{
	ERROR_AND_DIE("AddVertsForDisc2D not implemented yet");
}

//------------------------------------------------------------------------------
void AddVertsForRing2D( [[maybe_unused]] std::vector<Vertex>& verts, [[maybe_unused]] Vec2 ringCenter, [[maybe_unused]] float ringRadius, [[maybe_unused]] float thickness, [[maybe_unused]] Rgba8 color)
{
	ERROR_AND_DIE("AddVertsForRing2D not implemented yet");
}

//------------------------------------------------------------------------------
void AddVertsForAABB2D( std::vector<Vertex>& verts, AABB2 const& alignedBox, Rgba8 color )
{
	float minX = alignedBox.m_mins.x;
	float minY = alignedBox.m_mins.y;
	float maxX = alignedBox.m_maxs.x;
	float maxY = alignedBox.m_maxs.y;

	verts.push_back(Vertex(Vec3( minX, minY, 0.f ), color, Vec2( 0.f, 0.f )));
	verts.push_back(Vertex(Vec3( maxX, minY, 0.f ), color, Vec2( 1.f, 0.f )));
	verts.push_back(Vertex(Vec3( maxX, maxY, 0.f ), color, Vec2( 1.f, 1.f )));
	verts.push_back(Vertex(Vec3( minX, minY, 0.f ), color, Vec2( 0.f, 0.f )));
	verts.push_back(Vertex(Vec3( maxX, maxY, 0.f ), color, Vec2( 1.f, 1.f )));
	verts.push_back(Vertex(Vec3( minX, maxY, 0.f ), color, Vec2( 0.f, 1.f )));

}

//------------------------------------------------------------------------------
void AddVertsForOBB2D([[maybe_unused]] std::vector<Vertex>& verts, [[maybe_unused]] OBB2 const& orientedBox, [[maybe_unused]] Rgba8 color)
{
	ERROR_AND_DIE("AddVertsForOBB2D not implemented yet");
}

//------------------------------------------------------------------------------
void AddVertsForCapsule2D([[maybe_unused]] std::vector<Vertex>& verts, [[maybe_unused]] Vec2 boneStart, [[maybe_unused]] Vec2 boneEnd, [[maybe_unused]] float radius, [[maybe_unused]] Rgba8 color)
{
	ERROR_AND_DIE("AddVertsForCapsule2D not implemented yet");
}

//------------------------------------------------------------------------------
void AddVertsForTriangle2D([[maybe_unused]] std::vector<Vertex>& verts, [[maybe_unused]] Vec2 ccw0, [[maybe_unused]] Vec2 ccw1, [[maybe_unused]] Vec2 ccw2, [[maybe_unused]] Rgba8 color)
{
	ERROR_AND_DIE("AddVertsForTriangle2D not implemented yet");
}

//------------------------------------------------------------------------------
void AddVertsForLineSegment2D(std::vector<Vertex>& verts, Vec2 start, Vec2 end, Vec2 thickness, Rgba8 color)
{
	Vec2 directionLength = end - start;

	if ( directionLength.GetLengthSquared() == 0.f )
		return;

	Vec2 forwardNormal = directionLength.GetNormalized();
	Vec2 left = Vec2( -forwardNormal.y, forwardNormal.x );

	Vec2 halfThickness = left * ( thickness.x * 0.5f );

	Vec2 a = start - halfThickness;
	Vec2 b = start + halfThickness;
	Vec2 c = end + halfThickness;
	Vec2 d = end - halfThickness;

	// Add two triangles (6 vertices)
	verts.push_back( Vertex( Vec3( a.x, a.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( b.x, b.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( c.x, c.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );

	verts.push_back( Vertex( Vec3( a.x, a.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( c.x, c.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( d.x, d.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
}
