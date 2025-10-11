#include "Game/Interactable.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"

Interactable::Interactable(Game* owner, Vec2 const& startPos)
	: Entity(owner, startPos)
{
	//m_position = (g_rng.RollRandomFloatInRange(WORLD_SIZE_X, 0.f), g_rng.RollRandomFloatInRange(0.f, WORLD_SIZE_Y));
	m_game = owner;
	m_position.x = g_rng.RollRandomFloatInRange(WORLD_SIZE_X, 0.f);
	m_position.y = g_rng.RollRandomFloatInRange(0.f, WORLD_SIZE_Y);
	//m_position = Vec2(10.f, 5.f);
	m_cosmeticRadius = INTERACTABLES_COSMETIC_RADIUS;
	m_physicsRadius = INTERACTABLES_PHYSICS_RADIUS;
	InitializeLocalVerts();
}

//------------------------------------------------------------------------------
Interactable::~Interactable()
{

}

void Interactable::Update(float deltaSeconds)
{
	InitializeLocalVerts();
}

void Interactable::Render() const
{
	if (m_isDead)
	{
		return;
	}
	
	Vertex tempWorldVerts[NUM_INTERACTABLE_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_INTERACTABLE_VERTS; ++vertIndex)
	{
		tempWorldVerts[vertIndex] = m_interactableVerts[vertIndex];
	}
	
	TransformVertexArrayXY3D(NUM_INTERACTABLE_VERTS, tempWorldVerts, 1.0f, 0.f, m_position);
	g_engine->m_render->DrawVertexArray(NUM_INTERACTABLE_VERTS, tempWorldVerts);
	
	if (m_game->g_drawDebug)
		DebugRender();
}

//------------------------------------------------------------------------------
void Interactable::InitializeLocalVerts()
{
	float holeRadii[NUM_BLACK_HOLE_SIDES] = {};
	for (int sideNum = 0; sideNum < NUM_BLACK_HOLE_SIDES; ++sideNum)
	{
		holeRadii[sideNum] = g_rng.RollRandomFloatInRange(1.9f, 2.0f);
	}

	// Precompute 2D vertex offsets
	constexpr float degreesPerBlackHoleSide = 360.f / (float)NUM_BLACK_HOLE_SIDES;
	Vec2 blackholeLocalVertPositions[NUM_BLACK_HOLE_SIDES] = {};
	for (int sideNum = 0; sideNum < NUM_BLACK_HOLE_SIDES; ++sideNum)
	{
		float degrees = degreesPerBlackHoleSide * (float)sideNum;
		float radius = holeRadii[sideNum];
		blackholeLocalVertPositions[sideNum].x = radius * CosDegrees(degrees);
		blackholeLocalVertPositions[sideNum].y = radius * SinDegrees(degrees);
	}

	// Build triangles
	for (int triNum = 0; triNum < NUM_BLACK_HOLE_TRIS; ++triNum)
	{
		int startRadiusIndex = triNum;
		int endRadiusIndex = (triNum + 1) % NUM_BLACK_HOLE_SIDES;
		int firstVertIndex = (triNum * 3) + 0;
		int secondVertIndex = (triNum * 3) + 1;
		int thirdVertIndex = (triNum * 3) + 2;
		Vec2 secondVertOfs = blackholeLocalVertPositions[startRadiusIndex];
		Vec2 thirdVertOfs = blackholeLocalVertPositions[endRadiusIndex];
		m_interactableVerts[firstVertIndex].m_position = Vec3(0.f, 0.f, 0.f);
		m_interactableVerts[secondVertIndex].m_position = Vec3(secondVertOfs.x, secondVertOfs.y, 0.f);
		m_interactableVerts[thirdVertIndex].m_position = Vec3(thirdVertOfs.x, thirdVertOfs.y, 0.f);
	}

	// Set colors
	for (int triNum = 0; triNum < NUM_BLACK_HOLE_TRIS; ++triNum)
	{
		int firstVertIndex = (triNum * 3) + 0;
		int secondVertIndex = (triNum * 3) + 1;
		int thirdVertIndex = (triNum * 3) + 2;

		// Center vertex
		m_interactableVerts[firstVertIndex].m_color = Rgba8(255, 255, 255, 255);

		// Edge vertices
		m_interactableVerts[secondVertIndex].m_color = Rgba8(0, 0, 0, 255);
		m_interactableVerts[thirdVertIndex].m_color = Rgba8(0, 0, 0, 255);
	}
}


//-----------------------------------------------------------------------------------------------
void Interactable::Die()
{
	SoundPlaybackID temp = g_engine->m_audio->StartSound(7, false, 0.3f);
	m_game->HandleSound(temp, PRIORITY_MEDIUM, 0.2f);
	m_isGarbage = true;
	m_isDead = true;
	g_theApp->m_game->SpawnDebrisCluster(m_position, m_entityColor, m_velocity, m_debrisAmount, m_debrisSize);
}
