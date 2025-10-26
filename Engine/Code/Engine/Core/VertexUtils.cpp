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
	// Get the four corners of the AABB
	float minX = alignedBox.m_mins.x;
	float minY = alignedBox.m_mins.y;
	float maxX = alignedBox.m_maxs.x;
	float maxY = alignedBox.m_maxs.y;

	int startIndex = static_cast<int>(verts.size());
	verts.resize(verts.size() + 6);

	verts[startIndex + 0].m_position = Vec3(minX, minY, 0.f);
	verts[startIndex + 1].m_position = Vec3(maxX, minY, 0.f);
	verts[startIndex + 2].m_position = Vec3(maxX, maxY, 0.f);

	verts[startIndex + 3].m_position = Vec3(minX, minY, 0.f);
	verts[startIndex + 4].m_position = Vec3(maxX, maxY, 0.f);
	verts[startIndex + 5].m_position = Vec3(minX, maxY, 0.f);

	for (int vertIndex = 0; vertIndex < 6; ++vertIndex)
	{
		verts[startIndex + vertIndex].m_color = color;
	}
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
