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
void AddVertsForDisc2D(std::vector<Vertex>& verts, Vec2 const& center, float discRadius, Rgba8 color)
{
	int numWedges = 32;
	float degreesPerWedge = 360.f / static_cast<float>(numWedges);

	for (int wedgeIndex = 0; wedgeIndex < numWedges; ++wedgeIndex)
	{
		float startDegrees = degreesPerWedge * static_cast<float>(wedgeIndex);
		float endDegrees = degreesPerWedge * static_cast<float>(wedgeIndex + 1);

		Vec2 startPos = center + Vec2::MakeFromPolarDegrees(startDegrees, discRadius);
		Vec2 endPos = center + Vec2::MakeFromPolarDegrees(endDegrees, discRadius);

		verts.push_back(Vertex(Vec3(center.x, center.y, 0.f), color, Vec2(0.f, 0.f)));
		verts.push_back(Vertex(Vec3(startPos.x, startPos.y, 0.f), color, Vec2(0.f, 0.f)));
		verts.push_back(Vertex(Vec3(endPos.x, endPos.y, 0.f), color, Vec2(0.f, 0.f)));
	}
}

//------------------------------------------------------------------------------
void AddVertsForRing2D(std::vector<Vertex>& verts, Vec2 ringCenter, float ringRadius, float thickness, Rgba8 color)
{
	int numWedges = 32;
	float degreesPerWedge = 360.f / static_cast< float >( numWedges );

	float innerRadius = ringRadius - ( thickness * 0.5f );
	float outerRadius = ringRadius + ( thickness * 0.5f );

	for ( int wedgeIndex = 0; wedgeIndex < numWedges; ++wedgeIndex )
	{
		float startDegrees = degreesPerWedge * static_cast< float >( wedgeIndex );
		float endDegrees = degreesPerWedge * static_cast< float >( wedgeIndex + 1 );

		Vec2 innerStart = ringCenter + Vec2::MakeFromPolarDegrees( startDegrees, innerRadius );
		Vec2 innerEnd = ringCenter + Vec2::MakeFromPolarDegrees( endDegrees, innerRadius );

		Vec2 outerStart = ringCenter + Vec2::MakeFromPolarDegrees( startDegrees, outerRadius );
		Vec2 outerEnd = ringCenter + Vec2::MakeFromPolarDegrees( endDegrees, outerRadius );

		verts.push_back( Vertex( Vec3( innerStart.x, innerStart.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
		verts.push_back( Vertex( Vec3( outerStart.x, outerStart.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
		verts.push_back( Vertex( Vec3( outerEnd.x, outerEnd.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );

		verts.push_back( Vertex( Vec3( innerStart.x, innerStart.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
		verts.push_back( Vertex( Vec3( outerEnd.x, outerEnd.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
		verts.push_back( Vertex( Vec3( innerEnd.x, innerEnd.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	}
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
void AddVertsForOBB2D( std::vector<Vertex>& verts, OBB2 const& orientedBox, Rgba8 color )
{
	Vec2 jBasisNormal = orientedBox.m_iBasisNormal.GetRotatedBy90Degrees();

	Vec2 right = orientedBox.m_iBasisNormal * orientedBox.m_halfDimensions.x;
	Vec2 up = jBasisNormal * orientedBox.m_halfDimensions.y;

	Vec2 bottomLeft = orientedBox.m_center - right - up;
	Vec2 bottomRight = orientedBox.m_center + right - up;
	Vec2 topRight = orientedBox.m_center + right + up;
	Vec2 topLeft = orientedBox.m_center - right + up;

	verts.push_back( Vertex( Vec3( bottomLeft.x, bottomLeft.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( bottomRight.x, bottomRight.y, 0.f ), color, Vec2( 1.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( topRight.x, topRight.y, 0.f ), color, Vec2( 1.f, 1.f ) ) );

	verts.push_back( Vertex( Vec3( bottomLeft.x, bottomLeft.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( topRight.x, topRight.y, 0.f ), color, Vec2( 1.f, 1.f ) ) );
	verts.push_back( Vertex( Vec3( topLeft.x, topLeft.y, 0.f ), color, Vec2( 0.f, 1.f ) ) );
}

//------------------------------------------------------------------------------
void AddVertsForCapsule2D( std::vector<Vertex>& verts, Vec2 boneStart, Vec2 boneEnd, float radius, Rgba8 color )
{
	AddVertsForLineSegment2D( verts, boneStart, boneEnd, Vec2( radius * 2.f, radius * 2.f ), color );

	Vec2 direction = boneEnd - boneStart;
	Vec2 forward = direction.GetNormalized();

	int numWedges = 16;
	float degreesPerWedge = 180.f / static_cast< float >( numWedges );

	float forwardAngle = forward.GetOrientationDegrees();

	for ( int wedgeIndex = 0; wedgeIndex < numWedges; ++wedgeIndex )
	{
		float startDegrees = forwardAngle + 90.f + ( degreesPerWedge * static_cast< float >( wedgeIndex ) );
		float endDegrees = forwardAngle + 90.f + ( degreesPerWedge * static_cast< float >( wedgeIndex + 1 ) );

		Vec2 startPos = boneStart + Vec2::MakeFromPolarDegrees( startDegrees, radius );
		Vec2 endPos = boneStart + Vec2::MakeFromPolarDegrees( endDegrees, radius );

		verts.push_back( Vertex( Vec3( boneStart.x, boneStart.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
		verts.push_back( Vertex( Vec3( startPos.x, startPos.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
		verts.push_back( Vertex( Vec3( endPos.x, endPos.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	}

	for ( int wedgeIndex = 0; wedgeIndex < numWedges; ++wedgeIndex )
	{
		float startDegrees = forwardAngle - 90.f + ( degreesPerWedge * static_cast< float >( wedgeIndex ) );
		float endDegrees = forwardAngle - 90.f + ( degreesPerWedge * static_cast< float >( wedgeIndex + 1 ) );

		Vec2 startPos = boneEnd + Vec2::MakeFromPolarDegrees( startDegrees, radius );
		Vec2 endPos = boneEnd + Vec2::MakeFromPolarDegrees( endDegrees, radius );

		verts.push_back( Vertex( Vec3( boneEnd.x, boneEnd.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
		verts.push_back( Vertex( Vec3( startPos.x, startPos.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
		verts.push_back( Vertex( Vec3( endPos.x, endPos.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	}
}

//------------------------------------------------------------------------------
void AddVertsForTriangle2D(std::vector<Vertex>& verts, Vec2 ccw0, Vec2 ccw1, Vec2 ccw2, Rgba8 color)
{
	verts.push_back( Vertex( Vec3( ccw0.x, ccw0.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( ccw1.x, ccw1.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( ccw2.x, ccw2.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
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

	verts.push_back( Vertex( Vec3( a.x, a.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( b.x, b.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( c.x, c.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );

	verts.push_back( Vertex( Vec3( a.x, a.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( c.x, c.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
	verts.push_back( Vertex( Vec3( d.x, d.y, 0.f ), color, Vec2( 0.f, 0.f ) ) );
}
