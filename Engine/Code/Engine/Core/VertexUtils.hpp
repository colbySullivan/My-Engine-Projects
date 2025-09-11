#pragma once
#include "Vertex.hpp"

void TransformVertexArrayXY3D(int numVerts, Vertex* verts, float uniformScaleXY,
	float rotationDegreesAboutZ, Vec2 const& translationXY);