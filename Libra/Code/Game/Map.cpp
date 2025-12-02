#include "Game/Map.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/HeatMaps.hpp"
#include "Game/Scorpio.hpp"
#include "Game/Leo.hpp"
#include "Game/Aries.hpp"
#include "Game/Bullet.hpp"



//------------------------------------------------------------------------------
Map::Map(Game* game, MapDef const& mapDefinition)
	: m_game( game )
	, m_dimensions( mapDefinition.m_dimensions )
	, m_fillTileType( mapDefinition.m_fillTileType)
	, m_edgeTileType( mapDefinition.m_edgeTileType )
	, m_sprinkle1TileType( mapDefinition.m_sprinkle1TileType )
	, m_sprinkle2TileType( mapDefinition.m_sprinkle2TileType )
	, m_barrierTileType( mapDefinition.m_barrierTileType )
	//, m_distMapFromStartToPlayer( mapDefinition.m_dimensions )
{
	m_heatMap = new TileHeatMap( m_dimensions );
	m_numTilesInViewVertically = 10;
	m_debugCamera = false;
	CreateValidMapWithEntities();
}

//-----------------------------------------------------------------------------------------------
void Map::Update( float deltaSeconds)
{
	PushEntityOutOfEachOther();
	CheckLineOfSights();
	UpdateCameras();
	UpdateEntities( deltaSeconds );
	DestroyGarbageEntities();
	PopulateDijkstraMap( *m_heatMap, IntVec2(1,1), 999999.f);
}

//------------------------------------------------------------------------------
int Map::GetTileIndexForTileCoords(IntVec2 tileCoords) const
{
	return (tileCoords.y * m_dimensions.x) + tileCoords.x;
}

//------------------------------------------------------------------------------
IntVec2 Map::GetTileCoordsForIndex(int tileCoords) const
{
	int y = tileCoords / m_dimensions.x;
	int x = tileCoords % m_dimensions.x;
	return IntVec2(x, y);
}

//------------------------------------------------------------------------------
bool Map::IsTileSolidAtTileCoords(IntVec2 tileCoords) const
{
	if ( tileCoords.x < 0 || tileCoords.x >= m_dimensions.x ||
		tileCoords.y < 0 || tileCoords.y >= m_dimensions.y )
	{
		return true;
	}
	return m_tiles[GetTileIndexForTileCoords(tileCoords)].IsSolid();
}

//-----------------------------------------------------------------------------------------------
void Map::PushEntityOutOfEachOther() const
{
	for (int entityIndex = 0; entityIndex < m_allEntities.size() ; ++entityIndex)
	{
		Entity* firstEntity = m_allEntities[entityIndex];
		if ( firstEntity )
			for ( int otherEntityIndex = entityIndex + 1; otherEntityIndex < m_allEntities.size(); ++otherEntityIndex )
			{
				Entity* otherEntity = m_allEntities[otherEntityIndex];
				if ( otherEntity )
				{
					bool sameFaction = ( firstEntity->m_faction == otherEntity->m_faction );
					bool eitherIsBullet = ( firstEntity->m_entityType == ENTITY_TYPE_GOOD_BULLET ) || ( otherEntity->m_entityType == ENTITY_TYPE_GOOD_BULLET );
					if ( sameFaction && eitherIsBullet )
						continue;

					bool isBPushed = firstEntity->m_doesPushEntities && otherEntity->m_isPushedByEntities;
					bool isAPushed = otherEntity->m_doesPushEntities && firstEntity->m_isPushedByEntities;

					if ( isBPushed && !isAPushed )
					{
						PushDiscOutOfFixedDisc2D( otherEntity->m_position, otherEntity->m_physicsRadius, firstEntity->m_position, firstEntity->m_physicsRadius );
					}
					else if ( isAPushed && !isBPushed )
					{
						PushDiscOutOfFixedDisc2D( firstEntity->m_position, firstEntity->m_physicsRadius, otherEntity->m_position, otherEntity->m_physicsRadius );
					}
					else if ( isBPushed && isAPushed )
					{
						PushDiscsOutOfEachOther2D( firstEntity->m_position, firstEntity->m_physicsRadius, otherEntity->m_position, otherEntity->m_physicsRadius );
					}
					
				}
			}
	}
}

//-----------------------------------------------------------------------------------------------
void Map::PushEntityOutOfTileIfSolid( Entity& e, IntVec2 const& tileCoords )
{
	if ( !IsTileSolidAtTileCoords( tileCoords ) )
	{
		return;
	}

	AABB2 tileBounds = GetTileBounds( tileCoords );
	Vec2 nearestPointOnTile = tileBounds.GetNearestPoint( e.m_position );
	PushDiscOutOfFixedPoint2D( e.m_position, e.m_physicsRadius, nearestPointOnTile );
}

//-----------------------------------------------------------------------------------------------
void Map::PushEntityOutOfWalls( Entity& e, [[maybe_unused]] float deltaSeconds )
{
	if ( e.m_noClip && e.IsPlayer() )
	{
		return;
	}

	IntVec2 myTileCoords = GetTileCoordsForWorldPos( e.m_position );


	PushEntityOutOfTileIfSolid( e, myTileCoords + STEP_EAST );
	PushEntityOutOfTileIfSolid( e, myTileCoords + STEP_WEST );
	PushEntityOutOfTileIfSolid( e, myTileCoords + STEP_NORTH );
	PushEntityOutOfTileIfSolid( e, myTileCoords + STEP_SOUTH );

	PushEntityOutOfTileIfSolid( e, myTileCoords + STEP_NE );
	PushEntityOutOfTileIfSolid( e, myTileCoords + STEP_NW );
	PushEntityOutOfTileIfSolid( e, myTileCoords + STEP_SE );
	PushEntityOutOfTileIfSolid( e, myTileCoords + STEP_SW );
}

//------------------------------------------------------------------------------
AABB2 Map::GetTileBounds(IntVec2 const& tileCoords) const
{
	float minX = static_cast<float>(tileCoords.x);
	float minY = static_cast<float>(tileCoords.y);
	float maxX = minX + 1.0f;
	float maxY = minY + 1.0f;

	return AABB2(minX, minY, maxX, maxY);
}

IntVec2 Map::GetTileCoordsForWorldPos( Vec2 const& worldPos ) const
{
	int tileX = static_cast< int >( floorf( worldPos.x ) );
	int tileY = static_cast< int >( floorf( worldPos.y ) );
	return IntVec2( tileX, tileY );
}
//-----------------------------------------------------------------------------------------------
// Render
//-----------------------------------------------------------------------------------------------
void Map::Render() const
{
	SwtichMapRenderMode();
	RenderEntities();
}

//-----------------------------------------------------------------------------------------------
void Map::UpdateCameras()
{
	Player* player = nullptr;
	for ( int i = 0; i < static_cast< int >( m_allEntities.size() ); i++ )
	{
		if ( m_allEntities[i] && m_allEntities[i]->IsPlayer() )
		{
			player = ( Player* )m_allEntities[i];
			break;
		}
	}

	if ( player )
	{
		if ( m_debugCamera )
		{
			m_numTilesInViewVertically = m_dimensions.y;
		}
		else
		{
			m_numTilesInViewVertically = 10;
		}

		Vec2 cameraCenter = player->m_position;
		float halfHeight = m_numTilesInViewVertically * 0.5f;
		float halfWidth = halfHeight * 2.0f;
		Vec2 cameraMin = cameraCenter - Vec2( halfWidth, halfHeight );
		Vec2 cameraMax = cameraCenter + Vec2( halfWidth, halfHeight );

		float cameraWidth = cameraMax.x - cameraMin.x;
		float cameraHeight = cameraMax.y - cameraMin.y;

		cameraMin.x = GetClamped( cameraMin.x, 0.f, static_cast< float >( m_dimensions.x ) - cameraWidth );
		cameraMax.x = cameraMin.x + cameraWidth;

		cameraMin.y = GetClamped( cameraMin.y, 0.f, static_cast< float >( m_dimensions.y ) - cameraHeight );
		cameraMax.y = cameraMin.y + cameraHeight;

		m_game->m_worldCamera->SetOrthoView( cameraMin, cameraMax );
	}
	
}

//-----------------------------------------------------------------------------------------------
void Map::RenderTiles() const
{
	//Texture* spriteSheetTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/Terrain_8x8.png" );
	//SpriteSheet* tilesSpriteSheet = new SpriteSheet( *spriteSheetTexture, IntVec2( 8, 8 ) );
	//g_engine->m_render->BindTexture( &tilesSpriteSheet->GetTexture() );
	std::vector<Vertex> tileVerts;
	int totalTiles = static_cast< int >( m_tiles.size() );
	tileVerts.reserve( totalTiles * 6 );

	for ( int tileIndex = 0; tileIndex < totalTiles; ++tileIndex )
	{
		Tile const& tile = m_tiles[tileIndex];

		TileDefinition const& tileDef = TileDefinition::s_definitions[tile.m_type];

		float minX = static_cast< float >( tile.m_tileCoords.x );
		float minY = static_cast< float >( tile.m_tileCoords.y );
		float maxX = minX + 1.0f;
		float maxY = minY + 1.0f;

		AABB2 box = AABB2( minX, minY, maxX, maxY );

		AddVertsForAABB2D( tileVerts, box, tileDef.m_tint, tileDef.m_uvs.m_mins, tileDef.m_uvs.m_maxs );
	}

	g_engine->m_render->BindTexture( &m_game->m_tilesSpriteSheet->GetTexture() );
	g_engine->m_render->DrawVertexArray( static_cast< int >( tileVerts.size() ), tileVerts.data() );
	g_engine->m_render->BindTexture( nullptr );

}

//-----------------------------------------------------------------------------------------------
void Map::RenderHeatMapTiles() const
{
	std::vector<Vertex> tileVerts2;
	AABB2 mapbox = AABB2( 0.0f, 0.0f, static_cast< float >( m_dimensions.x ), static_cast< float >( m_dimensions.y ) );
	FloatRange mapFloatRange = FloatRange( 0.0f, static_cast< float >( ( m_dimensions.x + m_dimensions.y ) ) );
	m_heatMap->AddVertsForDebugDraw( tileVerts2, mapbox, mapFloatRange );
	g_engine->m_render->DrawVertexArray( static_cast< int >( tileVerts2.size() ), tileVerts2.data() );
}

//-----------------------------------------------------------------------------------------------
void Map::RenderSolidMapTiles() const
{
	std::vector<Vertex> tileVerts2;
	AABB2 mapbox = AABB2( 0.0f, 0.0f, static_cast< float >( m_dimensions.x ), static_cast< float >( m_dimensions.y ) );
	FloatRange mapFloatRange = FloatRange( 0.0f, 0.0f );
	m_heatMap->AddVertsForDebugDraw( tileVerts2, mapbox, mapFloatRange );

	for ( int i = 0; i < static_cast< int >( m_entityListsByType[ENTITY_TYPE_EVIL_SCORPIO].size() ); i++ )
	{
		Entity* entity = m_entityListsByType[ENTITY_TYPE_EVIL_SCORPIO][i];
		if ( entity == nullptr )
			continue;

		Vec2 solidPosition = entity->m_position;
		m_heatMap->Set(IntVec2((int)solidPosition.x, (int)solidPosition.y ), 999999.f );
	}

	g_engine->m_render->DrawVertexArray( static_cast< int >( tileVerts2.size() ), tileVerts2.data() );
}


//-----------------------------------------------------------------------------------------------
void Map::RenderEntities() const
{
	for ( int i = 0; i < static_cast< int >( m_allEntities.size() ); i++ )
	{
		Entity* entity = m_allEntities[i];
		if ( entity == nullptr )
			continue;

		entity->Render();
	}
}

//-----------------------------------------------------------------------------------------------
void Map::BuildMapTiles()
{
	int totalTiles = m_dimensions.x * m_dimensions.y;
	m_tiles.resize(totalTiles);

	FillTile1Setup();
	SprinkleTileSetup();
	WormFillTiles();
	OutEdgeTileSetup();
	SpawnBarrierTileSetup();
}

//------------------------------------------------------------------------------
// Tile Setups
//------------------------------------------------------------------------------
void Map::SpawnBarrierTileSetup()
{
	// Bottom left
	for (int barrierBoxFloorX = 1; barrierBoxFloorX < 5 ; ++barrierBoxFloorX)
	{
		for ( int barrierBoxFloorY = 1; barrierBoxFloorY < 5; ++barrierBoxFloorY ) 
		{
			m_tiles[GetTileIndexForTileCoords( IntVec2( barrierBoxFloorX, barrierBoxFloorY ) )].m_type = "Concrete";
		}
	}	
	m_tiles[GetTileIndexForTileCoords(IntVec2(2, 4))].m_type = m_barrierTileType;
	m_tiles[GetTileIndexForTileCoords(IntVec2(3, 4))].m_type = m_barrierTileType;
	m_tiles[GetTileIndexForTileCoords(IntVec2(4, 4))].m_type = m_barrierTileType;
	m_tiles[GetTileIndexForTileCoords(IntVec2(4, 3))].m_type = m_barrierTileType;
	m_tiles[GetTileIndexForTileCoords(IntVec2(4, 2))].m_type = m_barrierTileType;

	// Top right
	for ( int barrierBoxFloorX = m_dimensions.x - 5; barrierBoxFloorX < m_dimensions.x - 1; ++barrierBoxFloorX )
	{
		for ( int barrierBoxFloorY = m_dimensions.y - 5; barrierBoxFloorY < m_dimensions.y - 1; ++barrierBoxFloorY )
		{
			m_tiles[GetTileIndexForTileCoords( IntVec2( barrierBoxFloorX, barrierBoxFloorY ) )].m_type = "Concrete";
		}
	}
	m_tiles[GetTileIndexForTileCoords( IntVec2( m_dimensions.x - 3, m_dimensions.y - 5 ) )].m_type = m_barrierTileType;
	m_tiles[GetTileIndexForTileCoords( IntVec2( m_dimensions.x - 4, m_dimensions.y - 5 ) )].m_type = m_barrierTileType;
	m_tiles[GetTileIndexForTileCoords( IntVec2( m_dimensions.x - 5, m_dimensions.y - 5 ) )].m_type = m_barrierTileType;
	m_tiles[GetTileIndexForTileCoords( IntVec2( m_dimensions.x - 5, m_dimensions.y - 3 ) )].m_type = m_barrierTileType;
	m_tiles[GetTileIndexForTileCoords( IntVec2( m_dimensions.x - 5, m_dimensions.y - 4 ) )].m_type = m_barrierTileType;

	m_tiles[GetTileIndexForTileCoords( IntVec2( 1, 1 ) )].m_type = "MapEntry";
	m_tiles[GetTileIndexForTileCoords( IntVec2( m_dimensions.x - 2, m_dimensions.y - 2 ) )].m_type = "MapExit";

}

//------------------------------------------------------------------------------
void Map::OutEdgeTileSetup()
{
	for (int tileY = 0; tileY < m_dimensions.y; ++tileY)
	{
		for (int tileX = 0; tileX < m_dimensions.x; ++tileX)
		{
			bool isEdge = (tileX == 0 || tileX == m_dimensions.x - 1 ||
				tileY == 0 || tileY == m_dimensions.y - 1);

			if (isEdge)
			{
				int tileIndex = GetTileIndexForTileCoords(IntVec2(tileX, tileY));
				m_tiles[tileIndex].m_type = m_edgeTileType;
			}
		}
	}
}

//------------------------------------------------------------------------------
void Map::SprinkleTileSetup()
{
	for (int tileY = 1; tileY < m_dimensions.y - 1; ++tileY)
	{
		for (int tileX = 1; tileX < m_dimensions.x - 1; ++tileX)
		{

			if (g_rng.RollRandomFloatZeroToOne() < 0.15f && (tileX > 5 || tileY > 5))
			{
				int tileIndex = GetTileIndexForTileCoords(IntVec2(tileX, tileY));
				m_tiles[tileIndex].m_type = m_sprinkle1TileType;
			}
			else if( g_rng.RollRandomFloatZeroToOne() < 0.05f && ( tileX > 5 || tileY > 5 ) )
			{
				int tileIndex = GetTileIndexForTileCoords( IntVec2( tileX, tileY ) );
				m_tiles[tileIndex].m_type = m_sprinkle2TileType;
			}
		}
	}
}

//------------------------------------------------------------------------------
void Map::FillTile1Setup()
{
	for (int tileY = 0; tileY < m_dimensions.y; ++tileY)
	{
		for (int tileX = 0; tileX < m_dimensions.x; ++tileX)
		{
			int tileIndex = GetTileIndexForTileCoords(IntVec2(tileX, tileY));
			m_tiles[tileIndex].m_tileCoords = IntVec2(tileX, tileY);
			m_tiles[tileIndex].m_type = m_fillTileType;
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Map::WormFillTiles()
{
	int numOfWorms = g_rng.RollRandomIntInRange( 10, 20 );
	for (int wormNum = 0; wormNum < numOfWorms ; ++wormNum)
	{
		IntVec2 spawnPos = GetRandomValidPointInMapIntVec2();
		int tileIndex = GetTileIndexForTileCoords( IntVec2( spawnPos.x, spawnPos.y ) );
		m_tiles[tileIndex].m_type = m_edgeTileType;
		int wormHealth = g_rng.RollRandomIntInRange( 3, 10 );
		while ( wormHealth >= 0 )
		{
			IntVec2 legalMoves[4] = {
				IntVec2( spawnPos.x, spawnPos.y + 1 ),
				IntVec2( spawnPos.x, spawnPos.y - 1 ),
				IntVec2( spawnPos.x + 1, spawnPos.y ),
				IntVec2( spawnPos.x - 1, spawnPos.y )
			};
			int move = g_rng.RollRandomIntInRange( 0, 3 );
			spawnPos = legalMoves[move];
			int newtileIndex = GetTileIndexForTileCoords( IntVec2( spawnPos.x, spawnPos.y ) );
			if ( newtileIndex > 0 && newtileIndex < m_tiles.size() )
				m_tiles[newtileIndex].m_type = m_sprinkle2TileType;
			wormHealth--;
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Map::UpdatePlayerDevControls( XboxController const& controller )
{
	Entity* player = m_entityListsByType[ENTITY_TYPE_GOOD_PLAYER][0];

	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F3 ) )
	{
		player->m_noClip = !player->m_noClip;
	}

	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F2 ) )
	{

		player->m_isProtected = !player->m_isProtected;
	}

	if (( g_engine->m_input->WasKeyJustPressed( 'R' ) || controller.WasButtonJustPressed(XboxButtonID::A) ) && player->m_isDead )
	{
		player->Respawn();
	}

	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F4 ) )
	{
		m_debugCamera = !m_debugCamera;
	}
}

//-----------------------------------------------------------------------------------------------
bool Map::HasLineOfSight( Vec2 posA, Vec2 posB ) const
{
	Vec2 fwdNormal = ( posB - posA ).GetNormalized();
	float maxDist = GetDistance2D( posA, posB );
	RaycastResult2D result = RaycastVsTiles( posA, fwdNormal, maxDist );
	return !result.m_didImpact;
}

//-----------------------------------------------------------------------------------------------
RaycastResult2D Map::RaycastVsTiles( Vec2 startPos, Vec2 fwdNormal, float maxDist ) const
{
	constexpr float TINY_STEPS_PER_UNIT = 10.f;
	int numSteps = static_cast<int>(maxDist * TINY_STEPS_PER_UNIT);
	Vec2 tinyStepForward = fwdNormal * (maxDist / numSteps);
	Vec2 currentPos = startPos;
	RaycastResult2D hitResult;
	for (int i = 0; i < numSteps ; ++i)
	{
		currentPos += tinyStepForward;
		if ( IsTileSolidAtTileCoords( IntVec2(static_cast<int>(currentPos.x), static_cast<int>(currentPos.y) ) ) )
		{
			hitResult.m_didImpact = true;
			hitResult.m_impactPos = currentPos;
			hitResult.m_impactDist = GetDistance2D( startPos, currentPos );
			return hitResult;
		}
	}
	RaycastResult2D missResult;
	hitResult.m_didImpact = false;
	hitResult.m_impactPos = startPos + (fwdNormal * maxDist);
	hitResult.m_impactDist = maxDist;
	return hitResult;
}

//-----------------------------------------------------------------------------------------------
void Map::UpdateEntities( float deltaSeconds )
{
	for ( int i = 0; i < static_cast< int >( m_allEntities.size() ); i++ )
	{
		Entity* entity = m_allEntities[i];
		if ( entity == nullptr )
			continue;

		entity->Update( deltaSeconds );

		if ( entity->m_isPushedByWalls )
		{
			PushEntityOutOfWalls( *entity, deltaSeconds );
		}
	}
}

//-----------------------------------------------------------------------------------------------
Vec2 Map::GetRandomValidPointInMapVec2()
{
	IntVec2 newTileCoords = IntVec2(0,0);
	while ( IsTileSolidAtTileCoords( newTileCoords ) )
	{
		newTileCoords = IntVec2( g_rng.RollRandomIntInRange( 5, m_dimensions.x ), g_rng.RollRandomIntInRange( 5, m_dimensions.y ) );
	}
	return (Vec2( static_cast<float>(newTileCoords.x + 0.5f), static_cast<float>(newTileCoords.y + 0.5f)));
}

//-----------------------------------------------------------------------------------------------
IntVec2 Map::GetRandomValidPointInMapIntVec2()
{
	IntVec2 newTileCoords = IntVec2(0,0);
	while ( IsTileSolidAtTileCoords( newTileCoords ) )
	{
		newTileCoords = IntVec2( g_rng.RollRandomIntInRange( 1, m_dimensions.x ), g_rng.RollRandomIntInRange( 1, m_dimensions.y ) );
	}
	return newTileCoords;
}

//-----------------------------------------------------------------------------------------------
void Map::CheckLineOfSights()
{
	for ( int playerIndex = 0; playerIndex < m_entityListsByType[ENTITY_TYPE_GOOD_PLAYER].size(); ++playerIndex )
	{
		Entity* player = m_entityListsByType[ENTITY_TYPE_GOOD_PLAYER][playerIndex];
		if ( player )
		{
			for (int enemyIndex = 0; enemyIndex < m_allEntities.size() ; ++enemyIndex)
			{
				
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Map::SwtichMapRenderMode() const
{
	switch ( m_game->m_mapRenderMode )
	{
	case REGULAR_MAP_MODE:	RenderTiles();			break;
	case HEAT_MAP_MODE:		RenderHeatMapTiles();	break;
	case SOLID_MAP_MODE:	RenderSolidMapTiles();	break;

	default:
		break;
	}
}

//-----------------------------------------------------------------------------------------------
void Map::CreateValidMapWithEntities()
{
	IntVec2 endCheckpoint = IntVec2( m_dimensions.x - 2, m_dimensions.y - 2 );
	bool notValid = true;
	int invalidTries = 0;
	while ( notValid &&  invalidTries < 1000 )
	{
		BuildMapTiles();
		PopulateDijkstraMap( *m_heatMap, IntVec2( 1, 1 ), 999999.f );
		if ( m_heatMap->Get( endCheckpoint ) < 999999.f ) //TODO don't hardcode // special
		{
			FillInImpossibleTiles();
			CreateInitialEntities();
			notValid = false;
		}
		invalidTries++;
	}

}

//-----------------------------------------------------------------------------------------------
void Map::FillInImpossibleTiles()
{
	for ( int tileY = 0; tileY < m_dimensions.y; ++tileY )
	{
		for ( int tileX = 0; tileX < m_dimensions.x; ++tileX )
		{
			int tileIndex = GetTileIndexForTileCoords( IntVec2( tileX, tileY ) );
			if ( m_heatMap->Get( IntVec2( tileX, tileY ) ) == 999999.f && m_tiles[tileIndex].m_type == m_fillTileType )
			{
				m_tiles[tileIndex].m_type = m_sprinkle2TileType;
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Map::CreateInitialEntities()
{
	for ( int Index = 0; Index < 10; ++Index )
	{
		SpawnNewEntity( ENTITY_TYPE_EVIL_SCORPIO, GetRandomValidPointInMapVec2(), 0.f, FACTION_EVIL );
		SpawnNewEntity( ENTITY_TYPE_EVIL_LEO, GetRandomValidPointInMapVec2(), 0.f, FACTION_EVIL );
		SpawnNewEntity( ENTITY_TYPE_EVIL_ARIES, GetRandomValidPointInMapVec2(), 0.f, FACTION_EVIL );
	}
}

//-----------------------------------------------------------------------------------------------
void Map::PopulateDijkstraMap( TileHeatMap& out_dijkstraMap, IntVec2 startCoords, float maxCost, [[maybe_unused]] bool treatWaterAsSolid )
{
	out_dijkstraMap.SetAllValues( maxCost );
	out_dijkstraMap.Set( startCoords, 0.f );

	int mapSize = out_dijkstraMap.GetMapSize();
	bool changedAnyValue = true;
	int checkedValue = 0;
	while ( changedAnyValue )
	{
		changedAnyValue = false;

		for ( int mapIndex = 0; mapIndex < mapSize; ++mapIndex )
		{
			int xCoord = mapIndex % out_dijkstraMap.m_dimensions.x;
			int yCoord = mapIndex / out_dijkstraMap.m_dimensions.x;

			IntVec2 currentCoords( xCoord, yCoord );
			float currentValueAtCoord = out_dijkstraMap.Get( currentCoords );

			IntVec2 neighbors[4] = {
				IntVec2( xCoord, yCoord + 1 ),
				IntVec2( xCoord, yCoord - 1 ),
				IntVec2( xCoord + 1, yCoord ),
				IntVec2( xCoord - 1, yCoord ) 
			};
			if ( currentValueAtCoord == checkedValue )
			{
				for ( int i = 0; i < 4; ++i )
				{
					IntVec2 neighborCoords = neighbors[i];

					float newCost = checkedValue + 1.f;
					float neighborValue = out_dijkstraMap.Get( neighborCoords );

					if ( newCost < neighborValue && !IsTileSolidAtTileCoords( neighborCoords ) )
					{
						out_dijkstraMap.Set( neighborCoords, newCost );
						changedAnyValue = true;
					}
				}
			}
		}
		checkedValue++;
	}
}

//-----------------------------------------------------------------------------------------------
Entity* Map::SpawnNewEntity( EntityType type, Vec2 const& position, float orientationDegrees, EntityFaction faction )
{
	Entity* newEntity = nullptr;

	switch ( type )
	{
		case ENTITY_TYPE_GOOD_PLAYER:	newEntity = new Player( m_game, position, orientationDegrees, faction, this, ENTITY_TYPE_GOOD_PLAYER );	break;
		case ENTITY_TYPE_EVIL_LEO:		newEntity = new Leo( m_game, position, orientationDegrees, faction, this, ENTITY_TYPE_EVIL_LEO );		break;
		case ENTITY_TYPE_EVIL_SCORPIO:	newEntity = new Scorpio( m_game, position, orientationDegrees, faction, this, ENTITY_TYPE_EVIL_SCORPIO );	break;
		case ENTITY_TYPE_EVIL_ARIES:	newEntity = new Aries( m_game, position, orientationDegrees, faction, this, ENTITY_TYPE_EVIL_ARIES );	break;
		case ENTITY_TYPE_GOOD_BULLET:	newEntity = new Bullet( m_game, position, orientationDegrees, faction, this, ENTITY_TYPE_GOOD_BULLET );	break;
		default:																										return nullptr;
	}

	if ( newEntity )
	{
		AddEntityToMap( *newEntity );
	}

	return newEntity;
}


//-----------------------------------------------------------------------------------------------
void Map::AddEntityToMap( Entity& e )
{
	EntityType type = e.GetEntityType();
	m_entityListsByType[type].push_back( &e );
	m_allEntities.push_back( &e );
}

//-----------------------------------------------------------------------------------------------
void Map::RemoveEntityFromMap( Entity& e )
{
	EntityType type = e.GetEntityType();
	EntityList& listByType = m_entityListsByType[type];
	listByType.erase( std::remove( listByType.begin(), listByType.end(), &e ), listByType.end() );
	m_allEntities.erase( std::remove( m_allEntities.begin(), m_allEntities.end(), &e ), m_allEntities.end() );
}

//-----------------------------------------------------------------------------------------------
void Map::DestroyGarbageEntities()
{
	for (int entityIndex = 0; entityIndex < m_allEntities.size() ; ++entityIndex)
	{
		Entity* grabageEntity = m_allEntities[entityIndex];
		if ( grabageEntity && grabageEntity->m_isGarbage )
		{
			RemoveEntityFromMap(*grabageEntity);
		}
	}
}

//-----------------------------------------------------------------------------------------------
bool Map::IsPlayerOnPortal()
{
	Entity* player = m_entityListsByType[ENTITY_TYPE_GOOD_PLAYER][0];
	Vec2 portalPosition( m_dimensions.x - 2.f, m_dimensions.y - 2.f );
 	return ( player->m_position - portalPosition ).GetLengthSquared() < 0.5f;
}

//-----------------------------------------------------------------------------------------------
bool Map::IsPlayerAlive()
{
	Entity* player = m_entityListsByType[ENTITY_TYPE_GOOD_PLAYER][0];
	return !player->m_isDead;
}
