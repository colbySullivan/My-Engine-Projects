#include "Game/Map.hpp"
#include "Game/Player.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/DebugRender.hpp"

struct LightingConstants
{
	Vec3 SunDirection;
	float SunIntensity;
	float AmbientIntensity;
	Vec3  FakePad;
};

//-----------------------------------------------------------------------------------------------
Map::Map( Game* game, const MapDefinition* definition )
	: m_game( game )
	, m_texture( definition->m_spriteSheetTexture )
	, m_shader( definition->m_shader )
	, m_definition( definition )
{
	m_dimensions = m_definition->m_image.GetDimensions();

	m_sunDirection = Vec3( 2.f, 1.f, -1.f );
	m_sunIntensity = 0.1f;
	m_AmbientIntensity = 0.5f;

	CreateTiles();
	CreateGeometry();
	AddActors();
}

//-----------------------------------------------------------------------------------------------
Map::~Map()
{
	delete m_vertexBuffer;
	m_vertexBuffer = nullptr;

	delete m_indexBuffer;
	m_indexBuffer = nullptr;

	delete m_shader;
	m_shader = nullptr;

	delete m_lightingConstant;
	m_lightingConstant = nullptr;

	m_tiles.clear();
}

//------------------------------------------------------------------------------
void Map::CreateTiles()
{
	const Image& mapImage = m_definition->m_image;
	int numTiles = m_dimensions.x * m_dimensions.y;

	m_tiles.reserve( numTiles );

	for ( int y = 0; y < m_dimensions.y; ++y )
	{
		for ( int x = 0; x < m_dimensions.x; ++x )
		{
			Rgba8 pixelColor = mapImage.GetTexelColor( x, y );
			std::string tileTypeName = "DefaultTile";

			std::map<std::string, TileDefinition>::const_iterator it;
			for ( it = TileDefinition::s_definitions.begin(); it != TileDefinition::s_definitions.end(); ++it )
			{
				const std::string& name = it->first;
				const TileDefinition& def = it->second;

				if ( pixelColor == def.m_mapImagePixelColor )
				{
					tileTypeName = name;
					break;
				}
			}
			m_tiles.emplace_back( IntVec2( x, y ), tileTypeName );
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Map::AddGeometryForWall( const AABB3& bounds, const AABB2& UVs )
{
	Vec3 bl = Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z );
	Vec3 br = Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z );
	Vec3 tr = Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z );
	Vec3 tl = Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z );
	AddVertsForQuad3D( m_vertexes, m_indexes, bl, br, tr, tl, Rgba8(255, 255, 255, 255), UVs );

	bl = Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z );
	br = Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z );
	tr = Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z );
	tl = Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z );
	AddVertsForQuad3D( m_vertexes, m_indexes, bl, br, tr, tl, Rgba8( 255, 255, 255, 255 ), UVs );

	bl = Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z );
	br = Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z );
	tr = Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z );
	tl = Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z );
	AddVertsForQuad3D( m_vertexes, m_indexes, bl, br, tr, tl, Rgba8( 255, 255, 255, 255 ), UVs );

	bl = Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z );
	br = Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z );
	tr = Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z );
	tl = Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z );
	AddVertsForQuad3D( m_vertexes, m_indexes, bl, br, tr, tl, Rgba8( 255, 255, 255, 255 ), UVs );
}

//-----------------------------------------------------------------------------------------------
void Map::AddGeometryForFloor( const AABB3& bounds, const AABB2& UVs )
{
	Vec3 bl = Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z );
	Vec3 br = Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z );
	Vec3 tr = Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z );
	Vec3 tl = Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z );
	AddVertsForQuad3D( m_vertexes, m_indexes, bl, br, tr, tl, Rgba8(255, 255, 255, 255), UVs );
}

//-----------------------------------------------------------------------------------------------
void Map::AddGeometryForCeiling( const AABB3& bounds, const AABB2& UVs )
{
	Vec3 bl = Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z );
	Vec3 br = Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z );
	Vec3 tr = Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z );
	Vec3 tl = Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z );
	AddVertsForQuad3D( m_vertexes, m_indexes, bl, br, tr, tl, Rgba8( 255, 255, 255, 255 ), UVs );
}

//------------------------------------------------------------------------------
void Map::AddActors()
{
	m_actorVector.push_back( new Actor( m_game, Vec3( 7.5f, 8.5f, 0.25f ), Vec3( 7.5f, 8.5f, 1.f ), 0.5f, 32 ) );
	m_actorVector.push_back( new Actor( m_game, Vec3( 8.5f, 8.5f, 0.125f ), Vec3( 8.5f, 8.5f, 1.f ), 0.5f, 32 ) );
	m_actorVector.push_back( new Actor( m_game, Vec3( 9.5f, 8.5f, 0.0f ), Vec3( 9.5f, 8.5f, 1.f ), 0.5f, 32 ) );

	Actor* projectile = new Actor( m_game, Vec3( 5.5f, 8.5f, 0.0f ), Vec3( 5.5f, 8.5f, 0.125f ), 0.0625f, 32 );
	projectile->m_controlledByPlayer = true;
	projectile->m_actorType = PROJECTILE;
	projectile->m_canBePushed = true;
	m_actorVector.push_back( projectile );
}

void Map::CreateBuffers()
{
	if ( m_vertexBuffer ) 
	{
		delete m_vertexBuffer;
		m_vertexBuffer = nullptr;
	}
	if ( m_indexBuffer ) 
	{
		delete m_indexBuffer;
		m_indexBuffer = nullptr;
	}

	if ( m_vertexes.empty() )
	{
		return;
	}


	unsigned int vertexBufferSize = ( unsigned int )( m_vertexes.size() * sizeof( Vertex_PCUTBN ) );
	unsigned int vertexStride = ( unsigned int )sizeof( Vertex_PCUTBN );
	m_vertexBuffer = g_engine->m_render->CreateVertexBuffer( vertexBufferSize, vertexStride );

	unsigned int indexBufferSize = ( unsigned int )( m_indexes.size() * sizeof( unsigned int ) );
	m_indexBuffer = g_engine->m_render->CreateIndexBuffer( indexBufferSize );

	m_lightingConstant = g_engine->m_render->CreateConstantBuffer( sizeof( LightingConstants ) );

	g_engine->m_render->CopyCPUToGPU( m_vertexes.data(), vertexBufferSize, m_vertexBuffer );
	g_engine->m_render->CopyCPUToGPU( m_indexes.data(), indexBufferSize, m_indexBuffer );
}

//-----------------------------------------------------------------------------------------------
bool Map::AreCoordsInBounds( int x, int y ) const
{
	return ( x < m_dimensions.x && y < m_dimensions.y && x >= 0 && y >= 0 );
}

//-----------------------------------------------------------------------------------------------
const Tile* Map::GetTile( int x, int y ) const
{
	int tileIndex = (y * m_dimensions.x) + x;
	return &m_tiles[tileIndex];
	
}

//-----------------------------------------------------------------------------------------------
void Map::Update()
{
	for (int actorIndex = 0; actorIndex < m_actorVector.size() ; ++actorIndex)
	{
		m_actorVector[actorIndex]->Update( (float) g_engine->m_systemClock->GetDeltaSeconds() );
	}
	CollideActorsWithMap();
	CollideActors();
	Vec3 rayStart = m_game->m_player->m_position;
	Vec3 rayDir =  m_game->m_player->m_orientation.GetAsMatrix_IFwd_JLeft_KUp().GetIBasis3D();
	float rayDist = 10.f;

	RaycastResult3D result = RaycastAll( rayStart, rayDir, rayDist );

	if ( result.m_didImpact )
	{
		DebugAddWorldSphere( result.m_impactPos, 0.1f, 0.f, Rgba8( 255, 255, 0 ), Rgba8( 255, 255, 0 ) );
	}
}

//-----------------------------------------------------------------------------------------------
void Map::CollideActors()
{
	for (int actorIndex = 0; actorIndex < m_actorVector.size() ; ++actorIndex)
	{
		Actor* firstActor = m_actorVector[actorIndex];
		for (int otherActorIndex = actorIndex + 1; otherActorIndex < m_actorVector.size() ; ++otherActorIndex)
		{
			Actor* otherActor = m_actorVector[otherActorIndex];
			CollideActors( firstActor, otherActor );
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Map::CollideActors( Actor* actorA, Actor* actorB )
{
	Vec3 actorAEnd = actorA->m_position + Vec3( 0.f, 0.f, actorA->m_height );
	Vec3 actorBEnd = actorB->m_position + Vec3( 0.f, 0.f, actorB->m_height );

	if ( actorA->m_canBePushed && !actorB->m_canBePushed )
	{
		PushCylinderOutOfFixedCylinder( actorA->m_position, actorAEnd, actorA->m_radius,
			actorB->m_position, actorBEnd, actorB->m_radius );
	}
	else if ( !actorA->m_canBePushed && actorB->m_canBePushed )
	{
		PushCylinderOutOfFixedCylinder( actorB->m_position, actorBEnd, actorB->m_radius,
			actorA->m_position, actorAEnd, actorA->m_radius );
	}
}

//-----------------------------------------------------------------------------------------------
void Map::CollideActorsWithMap()
{
	for (int actorIndex = 0; actorIndex < m_actorVector.size() ; ++actorIndex)
	{
		Actor* actor = m_actorVector[actorIndex];
		if ( actor->m_controlledByPlayer )
		{
			CollideActorWithMap( actor );
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Map::CollideActorWithMap( Actor* actor )
{

	Vec2 actorLocation = Vec2( actor->m_position.x, actor->m_position.y );
	IntVec2 myTileCoords = GetTileCoordsForWorldPos( actorLocation );

	PushActorOutOfTileIfSolid( *actor, myTileCoords + STEP_EAST );
	PushActorOutOfTileIfSolid( *actor, myTileCoords + STEP_WEST );
	PushActorOutOfTileIfSolid( *actor, myTileCoords + STEP_NORTH );
	PushActorOutOfTileIfSolid( *actor, myTileCoords + STEP_SOUTH );

	PushActorOutOfTileIfSolid( *actor, myTileCoords + STEP_NE );
	PushActorOutOfTileIfSolid( *actor, myTileCoords + STEP_NW );
	PushActorOutOfTileIfSolid( *actor, myTileCoords + STEP_SE );
	PushActorOutOfTileIfSolid( *actor, myTileCoords + STEP_SW );
}

//-----------------------------------------------------------------------------------------------
IntVec2 Map::GetTileCoordsForWorldPos( Vec2 const& worldPos ) const
{
	int tileX = static_cast< int >( floorf( worldPos.x ) );
	int tileY = static_cast< int >( floorf( worldPos.y ) );
	return IntVec2( tileX, tileY );
}

//-----------------------------------------------------------------------------------------------
void Map::PushActorOutOfTileIfSolid( Actor& actor, IntVec2 const& tileCoords )
{
	if ( !IsTileSolidAtTileCoords( tileCoords ) )
	{
		return;
	}

	AABB2 tileBounds = GetTileBounds( tileCoords );
	Vec2 actorLocation = Vec2( actor.m_position.x, actor.m_position.y );
	Vec2 nearestPointOnTile = tileBounds.GetNearestPoint( actorLocation );
	PushDiscOutOfFixedPoint2D( actorLocation, actor.m_radius, nearestPointOnTile );
	actor.SetPosXY( actorLocation.x, actorLocation.y );
}

//-----------------------------------------------------------------------------------------------
int Map::GetTileIndexForTileCoords( IntVec2 tileCoords ) const
{
	return ( tileCoords.y * m_dimensions.x ) + tileCoords.x;
}

//-----------------------------------------------------------------------------------------------
AABB2 Map::GetTileBounds( IntVec2 const& tileCoords ) const
{
	float minX = static_cast< float >( tileCoords.x );
	float minY = static_cast< float >( tileCoords.y );
	float maxX = minX + 1.0f;
	float maxY = minY + 1.0f;

	return AABB2( minX, minY, maxX, maxY );
}

//-----------------------------------------------------------------------------------------------
bool Map::IsTileSolidAtTileCoords( IntVec2 tileCoords ) const
{
	if ( tileCoords.x < 0 || tileCoords.x >= m_dimensions.x ||
		tileCoords.y < 0 || tileCoords.y >= m_dimensions.y )
	{
		return true;
	}
	return m_tiles[GetTileIndexForTileCoords( tileCoords )].IsSolid();
}

void Map::Render() const
{
	if ( m_vertexBuffer == nullptr || m_indexBuffer == nullptr )
		return;

	g_engine->m_render->BindTexture( m_texture );

	SetLighting();
	
	g_engine->m_render->BindShader( m_shader );
	g_engine->m_render->SetModelConstants( Mat44(), Rgba8(255, 255, 255, 255) );

	unsigned int indexCount = ( unsigned int )m_indexes.size();
	g_engine->m_render->DrawIndexBuffer( m_vertexBuffer, m_indexBuffer, indexCount );

	for ( int actorIndex = 0; actorIndex < m_actorVector.size(); ++actorIndex )
	{
		Actor* currActor = m_actorVector[actorIndex];
		currActor->Render();
	}
}

//------------------------------------------------------------------------------
RaycastResult3D Map::RaycastAll( const Vec3& start, const Vec3& direction, float distance, Actor* owner /*= nullptr */ ) const
{
	RaycastResult3D closest;
	closest.m_didImpact = false;
	closest.m_impactDist = distance;
	closest.m_impactPos = start + ( direction * distance );

	RaycastResult3D xyResult = RaycastWorldXY( start, direction, distance );
	if ( xyResult.m_didImpact && xyResult.m_impactDist < closest.m_impactDist )
	{
		closest = xyResult;
	}

	RaycastResult3D zResult = RaycastWorldZ( start, direction, distance );
	if ( zResult.m_didImpact && zResult.m_impactDist < closest.m_impactDist )
	{
		closest = zResult;
	}

	for ( int actorIndex = 0; actorIndex < ( int )m_actorVector.size(); ++actorIndex )
	{
		Actor* actor = m_actorVector[actorIndex];
		if ( actor == owner )
		{
			continue;
		}

		Vec3 actorEnd = actor->m_position + Vec3( 0.f, 0.f, actor->m_height );
		//RaycastResult3D actorResult = TODO
	}

	return closest;
}

//------------------------------------------------------------------------------
RaycastResult3D Map::RaycastWorldXY( const Vec3& start, const Vec3& direction, float distance ) const
{
	constexpr float TINY_STEPS_PER_UNIT = 10.f;
	int numSteps = static_cast< int >( distance * TINY_STEPS_PER_UNIT );
	Vec3 tinyStepForward = direction * ( distance / numSteps );
	Vec3 currentPos = start;

	for ( int i = 0; i < numSteps; ++i )
	{
		currentPos += tinyStepForward;
		IntVec2 tileCoords( static_cast< int >( floorf( currentPos.x ) ),
			static_cast< int >( floorf( currentPos.y ) ) );
		if ( IsTileSolidAtTileCoords( tileCoords ) )
		{
			RaycastResult3D hitResult;
			hitResult.m_didImpact = true;
			hitResult.m_impactPos = currentPos;
			hitResult.m_impactDist = GetDistance3D( start, currentPos );
			return hitResult;
		}
	}

	RaycastResult3D missResult;
	missResult.m_didImpact = false;
	missResult.m_impactPos = start + ( direction * distance );
	missResult.m_impactDist = distance;
	return missResult;
}

RaycastResult3D Map::RaycastWorldZ( const Vec3& start, const Vec3& direction, float distance ) const
{
	RaycastResult3D missResult;
	return missResult;
}

void Map::SetLighting() const
{
	LightingConstants lightingConstants = { };
	lightingConstants.AmbientIntensity = m_AmbientIntensity;
	lightingConstants.SunDirection = Vec3(GetClamped(m_sunDirection.x, 0.f, 1.f), GetClamped(m_sunDirection.y, 0.f, 1.f), GetClamped(m_sunDirection.z, 0.f, 1.f) );
	lightingConstants.SunIntensity = GetClamped(m_sunIntensity, 0.f, 1.f);

	g_engine->m_render->BindConstantBuffer( 1, m_lightingConstant );
	g_engine->m_render->CopyCPUToGPU( &lightingConstants, sizeof( lightingConstants ), m_lightingConstant );
}

//------------------------------------------------------------------------------
void Map::CreateGeometry()
{
	m_vertexes.clear();
	m_indexes.clear();

	SpriteSheet terrainSheet( *m_texture, m_definition->m_spriteSheetCellCount );

	for ( int i = 0; i < ( int )m_tiles.size(); ++i )
	{
		//Tile const& tile = m_tiles[i];
		Tile& tile = m_tiles[i];

		if ( TileDefinition::s_definitions.count( tile.m_type ) == 0 )
		{
			continue;
		}

		const TileDefinition& def = TileDefinition::s_definitions.at( tile.m_type );

		float x = ( float )tile.m_tileCoords.x;
		float y = ( float )tile.m_tileCoords.y;
		AABB3 bounds( Vec3( x, y, 0.f ), Vec3( x + 1.f, y + 1.f, 1.f ) );
		tile.m_bounds = bounds;
		if ( def.m_isSolid )
		{
			AddGeometryForWall( bounds, terrainSheet.GetSpriteUVs( def.m_wallSpriteCoords ) );
		}
		else
		{
			if ( def.m_floorSpriteCoords.x != -1 && def.m_floorSpriteCoords.y != -1 )
			{
				AddGeometryForFloor( bounds, terrainSheet.GetSpriteUVs( def.m_floorSpriteCoords ) );
			}

			if ( def.m_ceilingSpriteCoords.x != -1 && def.m_ceilingSpriteCoords.y != -1 )
			{
				AddGeometryForCeiling( bounds, terrainSheet.GetSpriteUVs( def.m_ceilingSpriteCoords ) );
			}
		}
	}
	CreateBuffers();
}