#include "Game/Interactable.hpp"
#include "Game/PlayerShip.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"

Interactable::Interactable(Game* owner, Vec2 const& startPos)
	: Entity(owner, startPos)
{
	m_game = owner;
	m_position = startPos;
	m_cosmeticRadius = INTERACTABLES_COSMETIC_RADIUS;
	m_physicsRadius = INTERACTABLES_PHYSICS_RADIUS;
	InitializeLocalVerts();
}

//------------------------------------------------------------------------------
Interactable::~Interactable()
{

}

void Interactable::Update([[maybe_unused]] float deltaSeconds)
{
	InitializeLocalVerts();
}

void Interactable::Render() const
{
	if ( m_isDead )
	{
		return;
	}
	
	Vertex tempWorldVerts[NUM_INTERACTABLE_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_INTERACTABLE_VERTS; ++vertIndex)
	{
		tempWorldVerts[vertIndex] = m_interactableVerts[vertIndex];
	}
	
	TransformVertexArrayXY3D(NUM_INTERACTABLE_VERTS, tempWorldVerts, 4.0f, 0.f, m_position);
	g_engine->m_render->DrawVertexArray(NUM_INTERACTABLE_VERTS, tempWorldVerts);
	
	if ( m_game->g_drawDebug )
	{
		DebugRender();
	}
		
}

//------------------------------------------------------------------------------
void Interactable::InitializeLocalVerts()
{
	float holeRadii[NUM_BLACK_HOLE_SIDES] = {};
	for (int sideNum = 0; sideNum < NUM_BLACK_HOLE_SIDES; ++sideNum)
	{
		holeRadii[sideNum] = g_rng.RollRandomFloatInRange(1.8f, 2.0f);
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
		m_interactableVerts[firstVertIndex].m_color = Rgba8(100, 255, 150, 255);

		// Edge vertices
		m_interactableVerts[secondVertIndex].m_color = Rgba8(0, 200, 100, 255);
		m_interactableVerts[thirdVertIndex].m_color = Rgba8(0, 200, 100, 255);
	}
}


//-----------------------------------------------------------------------------------------------
void Interactable::Die()
{
	SoundPlaybackID temp = g_engine->m_audio->StartSound(7, false, 0.3f);
	m_game->HandleSound(temp, PRIORITY_MEDIUM, 0.2f);
	m_isGarbage = true;
	m_isDead = true;
	g_theApp->m_game->SpawnDebrisCluster(m_position, Rgba8(0, 200, 100, 255), m_velocity, m_debrisAmount, 0.5f);
}

PowerUp Interactable::ApplyEffect(PlayerShip* m_playerShip)
{
	if ( m_givenPowerUp )
	{
		return Num_PowerUps;
	}
		

	m_givenPowerUp = true; // Fixes issue of multiple powerups being given due to fast frame time

	PowerUp powerUpType = BulletSpeed1;
	int roll = g_rng.RollRandomIntInRange(0, 100);

	if		(roll < 20)	powerUpType = BulletSpeed1;
	else if (roll < 40)	powerUpType = BulletSpeed2;
	else if (roll < 45)	powerUpType = BulletSpeed3;
	else if (roll < 70)	powerUpType = BulletCount1;
	else if (roll < 90)	powerUpType = BulletCount2;
	else				powerUpType = BulletCount3;

	switch (powerUpType)
	{
	case BulletSpeed1: m_playerShip->m_bulletUpgradeSpeedDivisor -= 0.01f;	break;
	case BulletSpeed2: m_playerShip->m_bulletUpgradeSpeedDivisor -= 0.02f;	break;
	case BulletSpeed3: m_playerShip->m_bulletUpgradeSpeedDivisor -= 0.03f;	break;
	case BulletCount1: m_playerShip->m_bulletUpgradeAmount += 1;			break;
	case BulletCount2: m_playerShip->m_bulletUpgradeAmount += 2;			break;
	case BulletCount3: m_playerShip->m_hasBulletReverse = true;				break;
	default:																break;
	}
	return powerUpType;
}
