#pragma once
#include <string>
#include <map>
#include "Game/Tile.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Game/Actor.hpp"
#include "Game/ActorHandle.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/Controller.hpp"
#include "Game/WeaponDefinition.hpp"

class Game;
class Actor;
class Controller;

class Map
{
public:
	Map( Game* game, const MapDefinition* definition );
	~Map();

	void Startup();
	void Shutdown();

	Clock m_gameClock;

	void CreateTiles();
	void CreateGeometry();
	void AddGeometryForWall( const AABB3& bounds, const AABB2& UVs );
	void AddGeometryForFloor( const AABB3& bounds, const AABB2& UVs );
	void AddGeometryForCeiling( const AABB3& bounds, const AABB2& UVs );
	void AddActors();
	void CreateBuffers();

	bool IsPositionInBounds( const Vec3& position ) const;
	bool AreCoordsInBounds( int x, int y ) const;
	const Tile* GetTile( int x, int y ) const;

	void Update();
	void Render( Camera* playerCamera ) const;

	void CollideActors();
	void CollideActorAndProjectiles( Actor* actorA, Actor* actorB );
	void CollideActors( Actor* actorA, Actor* actorB );
	void CollideActorsWithMap();
	void CollideActorWithMap( Actor* actor );

	Actor* SpawnProjectileFromActor( Actor* owner, const WeaponDefinition& weaponDef, Vec3 direction );

	void PossessNextActor( Controller* controller );
	void SpawnPlayer( Controller* playerController );

	Actor* SpawnActor( const SpawnInfo& spawnInfo );
	Actor* GetActorByHandle( const ActorHandle handle ) const;
	Actor* FindActorAtPosition( const Vec3& position ) const;

	RaycastResult3D RaycastAll( const Vec3& start, const Vec3& direction, float distance, Actor* owner = nullptr ) const;
	RaycastResult3D RaycastWorldXY( const Vec3& start, const Vec3& direction, float distance ) const;
	RaycastResult3D RaycastWorldZ( const Vec3& start, const Vec3& direction, float distance ) const;
	RaycastResult3D RaycastWorldActors( const Vec3& start, const Vec3& direction, float distance, Actor* owner = nullptr ) const;

	void HandleDeath();
	void StartPickingPowerUp();
	void EndPickingPowerUp();
	bool IsCurrentlyPickingPowerUp() const { return m_isCurrentlyPickingPowerUp; }
	Game* m_game = nullptr;

	Vec3 m_sunDirection;
	float m_sunIntensity;
	float m_AmbientIntensity;
	std::vector<Actor*> m_actorVector;

protected:

	const MapDefinition* m_definition = nullptr;
	std::vector<Tile> m_tiles;
	IntVec2 m_dimensions;
	std::vector<Vertex_PCUTBN> m_vertexes;
	std::vector<unsigned int> m_indexes;
	Texture* m_texture = nullptr;
	Shader* m_shader = nullptr;
	VertexBuffer* m_vertexBuffer = nullptr;
	IndexBuffer* m_indexBuffer = nullptr;

	unsigned int m_nextActorUID = 0;
	bool		m_isCurrentlyPickingPowerUp = false;
	ConstantBuffer* m_lightingConstant;
private:
	void SetLighting() const;
	IntVec2 GetTileCoordsForWorldPos( Vec2 const& worldPos ) const;
	bool PushActorOutOfTileIfSolid( Actor& actor, IntVec2 const& tileCoords );
	int GetTileIndexForTileCoords( IntVec2 tileCoords ) const;
	AABB2 GetTileBounds( IntVec2 const& tileCoords ) const;
	bool IsTileSolidAtTileCoords( IntVec2 tileCoords ) const;
};