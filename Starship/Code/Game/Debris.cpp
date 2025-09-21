#include "Debris.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "Game/GameCommon.hpp"


Debris::Debris(Game* owner, Vec2 const& startPos)
	: Entity(owner, startPos)
{
	m_angularVelocity = g_rng.RollRandomFloatInRange(-200, 200);
	m_cosmeticRadius = DEBRIS_COSMETIC_RADIUS;
	m_physicsRadius = DEBRIS_PHYSICS_RADIUS;
	m_orientationDegrees = g_rng.RollRandomFloatInRange(0.1f, 360.f);
	m_position = startPos;
	m_health = 3;
	int randomVelocity = g_rng.RollRandomFloatInRange(2.1f, 5.f);
	m_velocity.x = randomVelocity * CosDegrees(m_orientationDegrees);
	m_velocity.y = randomVelocity * SinDegrees(m_orientationDegrees);

	InitializeLocalVerts();
}

Debris::~Debris()
{

}

void Debris::Update(float deltaSeconds)
{
	m_position += m_velocity * deltaSeconds;
	m_orientationDegrees += (m_angularVelocity * deltaSeconds);

	m_velocity *= 0.98f; // Slight deceleration

	m_timeoutTimer -= deltaSeconds;

	if (m_timeoutTimer > 0.0f)
	{
		unsigned char alpha = 127; // TODO Use functions in mathutil to fade

		for (int vertIndex = 0; vertIndex < NUM_DEBRIS_VERTS; ++vertIndex)
		{
			m_localVerts[vertIndex].m_color = Rgba8(100, 100, 100, alpha);
		}
	}

	if (m_health == 0 || m_timeoutTimer <= 0.0f)
	{
		m_isDead = true;
		m_isGarbage = true;
	}
}

void Debris::Render() const
{
	if (m_isDead)
		return;
	Vertex tempShipWorldVerts[NUM_DEBRIS_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_DEBRIS_VERTS; ++vertIndex)
	{
		tempShipWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}

	TransformVertexArrayXY3D(NUM_DEBRIS_VERTS, tempShipWorldVerts, 0.5f, m_orientationDegrees, m_position);
	g_engine->m_render->DrawVertexArray(NUM_DEBRIS_VERTS, tempShipWorldVerts);
	if (m_game->g_drawDebug)
		DebugRender();
}

void Debris::InitializeLocalVerts()
{
	// Precompute random radii along each triangle-seam (at each outer vertex)
	float debrisRadii[NUM_DEBRIS_SIDES] = {};
	for (int sideNum = 0; sideNum < NUM_DEBRIS_SIDES; ++sideNum)
	{
		debrisRadii[sideNum] = g_rng.RollRandomFloatInRange(m_physicsRadius, m_cosmeticRadius);
	}

	// Precompute 2D vertex offsets
	constexpr float degreesPerDebrisSide = 360.f / (float)NUM_DEBRIS_SIDES;
	Vec2 debrisLocalVertPositions[NUM_DEBRIS_SIDES] = {};
	for (int sideNum = 0; sideNum < NUM_DEBRIS_SIDES; ++sideNum)
	{
		float degrees = degreesPerDebrisSide * (float)sideNum;
		float radius = debrisRadii[sideNum];
		debrisLocalVertPositions[sideNum].x = radius * CosDegrees(degrees);
		debrisLocalVertPositions[sideNum].y = radius * SinDegrees(degrees);
	}

	// Build triangles
	for (int triNum = 0; triNum < NUM_DEBRIS_TRIS; ++triNum)
	{
		int startRadiusIndex = triNum;
		int endRadiusIndex = (triNum + 1) % NUM_DEBRIS_SIDES;
		int firstVertIndex = (triNum * 3) + 0;
		int secondVertIndex = (triNum * 3) + 1;
		int thirdVertIndex = (triNum * 3) + 2;
		Vec2 secondVertOfs = debrisLocalVertPositions[startRadiusIndex];
		Vec2 thirdVertOfs = debrisLocalVertPositions[endRadiusIndex];
		m_localVerts[firstVertIndex].m_position = Vec3(0.f, 0.f, 0.f);
		m_localVerts[secondVertIndex].m_position = Vec3(secondVertOfs.x, secondVertOfs.y, 0.f);
		m_localVerts[thirdVertIndex].m_position = Vec3(thirdVertOfs.x, thirdVertOfs.y, 0.f);
	}

	// Set colors
	for (int vertIndex = 0; vertIndex < NUM_DEBRIS_VERTS; ++vertIndex)
	{
		m_localVerts[vertIndex].m_color = Rgba8(100, 100, 100, 127);
	}
}

