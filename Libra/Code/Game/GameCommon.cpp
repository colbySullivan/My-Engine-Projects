#include "GameCommon.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include <Engine/Core/Vertex.hpp>
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "App.hpp"


//-----------------------------------------------------------------------------------------------
void DebugDrawRing(Vec2 const& center, float radius, float thickness, Rgba8 const& color)
{
	float halfThickness = 0.5f * thickness;
	float innerRadius = radius - halfThickness;
	float outerRadius = radius + halfThickness;
	constexpr int NUM_SIDES = 32;
	constexpr int NUM_TRIS = 2 * NUM_SIDES;
	constexpr int RING_NUM_VERTS  = 3 * NUM_TRIS; // Renamed to suppress mulitple decleration error
	Vertex verts[RING_NUM_VERTS ];
	constexpr float DEGREES_PER_SIDE = 360.f / static_cast<float>( NUM_SIDES );
	for (int sideNum = 0; sideNum < NUM_SIDES; ++sideNum)
	{
		// Compute angle-related terms
		float startDegrees = DEGREES_PER_SIDE * static_cast<float>(sideNum);
		float endDegrees = DEGREES_PER_SIDE * static_cast<float>(sideNum + 1);
		float cosStart = CosDegrees(startDegrees);
		float sinStart = SinDegrees(startDegrees);
		float cosEnd = CosDegrees(endDegrees);
		float sinEnd = SinDegrees(endDegrees);

		// Compute inner and outer positions
		Vec3 innerStartPos = Vec3(center.x + innerRadius * cosStart, center.y + innerRadius * sinStart, 0.0f);
		Vec3 outerStartPos = Vec3(center.x + outerRadius * cosStart, center.y + outerRadius * sinStart, 0.0f);
		Vec3 outerEndPos = Vec3(center.x + outerRadius * cosEnd, center.y + outerRadius * sinEnd, 0.0f);
		Vec3 innerEndPos = Vec3(center.x + innerRadius * cosEnd, center.y + innerRadius * sinEnd, 0.0f);

		// Trapezoid is made of two triangles: ABC and DEF
		// A is inner end; B is inner Start; C is outer Start
		// D is inner end; E is outer Start; F is outer end
		int vertIndexA = (6 * sideNum) + 0;
		int vertIndexB = (6 * sideNum) + 1;
		int vertIndexC = (6 * sideNum) + 2;
		int vertIndexD = (6 * sideNum) + 3;
		int vertIndexE = (6 * sideNum) + 4;
		int vertIndexF = (6 * sideNum) + 5;

		verts[vertIndexA].m_position = innerEndPos;
		verts[vertIndexB].m_position = innerStartPos;
		verts[vertIndexC].m_position = outerStartPos;
		verts[vertIndexA].m_color = color;
		verts[vertIndexB].m_color = color;
		verts[vertIndexC].m_color = color;

		verts[vertIndexD].m_position = innerEndPos;
		verts[vertIndexE].m_position = outerStartPos;
		verts[vertIndexF].m_position = outerEndPos;
		verts[vertIndexD].m_color = color;
		verts[vertIndexE].m_color = color;
		verts[vertIndexF].m_color = color;
	}
	g_engine->m_render->DrawVertexArray(RING_NUM_VERTS , verts);
}

//-----------------------------------------------------------------------------------------------
void DebugDrawLine(Vec2 const& entity1Pos, Vec2 const& entity2Pos, float thickness, Rgba8 const& color)
{
	float halfThickness = 0.5f * thickness;
	Vec2 direction = entity1Pos - entity2Pos;

	Vec2 leftNormalizeDirection = direction.GetNormalized().GetRotatedBy90Degrees();
	Vec2 halfThicknessOffset = leftNormalizeDirection * halfThickness;

	Vec2 bottomLeft = entity1Pos - halfThicknessOffset;
	Vec2 topLeft = entity1Pos + halfThicknessOffset;
	Vec2 topRight = entity2Pos + halfThicknessOffset;
	Vec2 bottomRight = entity2Pos - halfThicknessOffset;

	Vertex verts[NUM_PLAYER_VERTS];

	// First triangle
	verts[0].m_position = Vec3(bottomLeft.x, bottomLeft.y, 0.0f);
	verts[1].m_position = Vec3(topLeft.x, topLeft.y, 0.0f);
	verts[2].m_position = Vec3(topRight.x, topRight.y, 0.0f);

	// Second triangle
	verts[3].m_position = Vec3(bottomLeft.x, bottomLeft.y, 0.0f);
	verts[4].m_position = Vec3(topRight.x, topRight.y, 0.0f);
	verts[5].m_position = Vec3(bottomRight.x, bottomRight.y, 0.0f);

	for (int vertIndex = 0; vertIndex < NUM_PLAYER_VERTS; ++vertIndex)
	{
		verts[vertIndex].m_color = color;
	}

	g_engine->m_render->DrawVertexArray(NUM_PLAYER_VERTS, verts);
}
