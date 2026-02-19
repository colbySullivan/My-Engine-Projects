#pragma once
#include "Engine/Core/Vertex.hpp"
#include <vector>

//-----------------------------------------------------------------------------------------------
struct AABB2;
struct OBB2;

//-----------------------------------------------------------------------------------------------
void TransformVertexArrayXY3D(int numVerts, Vertex* verts, float uniformScaleXY,
	float rotationDegreesAboutZ, Vec2 const& translationXY);

//-----------------------------------------------------------------------------------------------
void AddVertsForDisc2D(std::vector<Vertex>& verts, Vec2 const& center, float discRadius, Rgba8 color); // Also maybe numSides?
void AddVertsForRing2D(std::vector<Vertex>& verts, Vec2 ringCenter, float ringRadius, float thickness, Rgba8 color);
void AddVertsForAABB2D(std::vector<Vertex>& verts, AABB2 const& alignedBox, Rgba8 color);
void AddVertsForAABB2D( std::vector<Vertex>& verts, AABB2 const& alignedBox, Rgba8 color, Vec2 const& uvAtMins, Vec2 const& uvAtMaxs );
void AddVertsForOBB2D( std::vector<Vertex>& verts, OBB2 const& orientedBox, Rgba8 color );
void AddVertsForCapsule2D(std::vector<Vertex>& verts, Vec2 boneStart, Vec2 boneEnd, float radius, Rgba8 color);
void AddVertsForTriangle2D(std::vector<Vertex>& verts, Vec2 ccw0, Vec2 ccw1, Vec2 ccw2, Rgba8 color);
void AddVertsForLineSegment2D(std::vector<Vertex>& verts, Vec2 start, Vec2 end, Vec2 thickness, Rgba8 color);
void AddVertsForArrow2D( std::vector<Vertex>& verts, Vec2 tailPos, Vec2 tipPos, float arrowSize, float lineThickness, Rgba8 color );
//void AddVertsForQuad3D( std::vector<Vertex>& verts, const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft, const Rgba8& color = Rgba8::WHITE, const AABB2& UVs = AABB2::ZERO_TO_ONE );