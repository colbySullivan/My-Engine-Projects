#include "VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"



void TransformVertexArrayXY3D(int numVerts, Vertex* verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY)
{
	Vec2 iBasis = Vec2::MakeFromPolarDegrees( rotationDegreesAboutZ );
	Vec2 jBasis = iBasis.GetRotatedBy90Degrees();
	for (int vertIndex = 0; vertIndex < numVerts; ++vertIndex)
	{
		Vec3& pos = verts[vertIndex].m_position;
		TransformPositionXY3D(pos, uniformScaleXY, rotationDegreesAboutZ, translationXY); // Old version
		//TransformPositionXY3D(pos, iBasis, jBasis, translationXY);
	}
}
