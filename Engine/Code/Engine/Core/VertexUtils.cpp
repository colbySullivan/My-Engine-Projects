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
	ERROR_AND_DIE("AddVertsForDisc2D not implemented yet");
}

//------------------------------------------------------------------------------
void AddVertsForRing2D(std::vector<Vertex>& verts, Vec2 ringCenter, float ringRadius, float thickness, Rgba8 color)
{
	ERROR_AND_DIE("AddVertsForRing2D not implemented yet");
}

//------------------------------------------------------------------------------
void AddVertsForAABB2D(std::vector<Vertex>& verts, AABB2 const& alignedBox, Rgba8 color)
{
	ERROR_AND_DIE("AddVertsForAABB2D not implemented yet");
}

//------------------------------------------------------------------------------
void AddVertsForOBB2D(std::vector<Vertex>& verts, OBB2 const& orientedBox, Rgba8 color)
{
	ERROR_AND_DIE("AddVertsForOBB2D not implemented yet");
}

//------------------------------------------------------------------------------
void AddVertsForCapsule2D(std::vector<Vertex>& verts, Vec2 boneStart, Vec2 boneEnd, float radius, Rgba8 color)
{
	ERROR_AND_DIE("AddVertsForCapsule2D not implemented yet");
}

//------------------------------------------------------------------------------
void AddVertsForArrow2D(std::vector<Vertex>& verts, Vec2 ccw1, Vec2 ccw2, Rgba8 color)
{
	ERROR_AND_DIE("AddVertsForArrow2D not implemented yet");
}

//------------------------------------------------------------------------------
void AddVertsForLineSegment2D(std::vector<Vertex>& verts, Vec2 start, Vec2 end, Vec2 thickness, Rgba8 color)
{
	ERROR_AND_DIE("AddVertsForLineSegment2D not implemented yet");
}
