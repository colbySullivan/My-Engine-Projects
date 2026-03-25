#include "Game/Map.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"

//-----------------------------------------------------------------------------------------------
Map::Map( Game* game, const MapDefinition* definition )
	: m_game( game )
	, m_texture( definition->m_spriteSheetTexture )
	, m_shader( definition->m_shader )
	, m_definition( definition )
{
	m_dimensions = m_definition->m_image.GetDimensions();
	CreateTiles();
	CreateGeometry();
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
			std::string tileTypeName = "StoneFloor";

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
	AddVertsForAABB3D( m_vertexes, m_indexes, bounds, Rgba8(255, 255, 255, 255), UVs );
}

//-----------------------------------------------------------------------------------------------
void Map::AddGeometryForFloor( const AABB3& bounds, const AABB2& UVs )
{
	AABB3 floorSlab( Vec3( bounds.m_mins.x, bounds.m_mins.y, 0.f ), Vec3( bounds.m_maxs.x, bounds.m_maxs.y, 0.f ) );
	AddVertsForAABB3D( m_vertexes, m_indexes, floorSlab, Rgba8(255, 255, 255, 255), UVs );
}

//-----------------------------------------------------------------------------------------------
void Map::AddGeometryForCeiling( const AABB3& bounds, const AABB2& UVs )
{
	AABB3 ceilSlab( Vec3( bounds.m_mins.x, bounds.m_mins.y, 1.0f ), Vec3( bounds.m_maxs.x, bounds.m_maxs.y, 1.0f ) );
	AddVertsForAABB3D( m_vertexes, m_indexes, ceilSlab, Rgba8(255, 255, 255, 255), UVs );
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

	g_engine->m_render->CopyCPUToGPU( m_vertexes.data(), vertexBufferSize, m_vertexBuffer );
	g_engine->m_render->CopyCPUToGPU( m_indexes.data(), indexBufferSize, m_indexBuffer );
}

void Map::Render() const
{
	if ( m_vertexBuffer == nullptr || m_indexBuffer == nullptr )
		return;

	g_engine->m_render->BindTexture( m_texture );

	//g_engine->m_render->BindShader( m_shader ); // #TODO this is the diffuse shader and should be implemented later
	g_engine->m_render->SetModelConstants( Mat44(), Rgba8(255, 255, 255, 255) );

	unsigned int indexCount = ( unsigned int )m_indexes.size();
	g_engine->m_render->DrawIndexBuffer( m_vertexBuffer, m_indexBuffer, indexCount );
}

//------------------------------------------------------------------------------
void Map::CreateGeometry()
{
	m_vertexes.clear();
	m_indexes.clear();

	SpriteSheet terrainSheet( *m_texture, m_definition->m_spriteSheetCellCount );

	for ( int i = 0; i < ( int )m_tiles.size(); ++i )
	{
		Tile const& tile = m_tiles[i];

		if ( TileDefinition::s_definitions.count( tile.m_type ) == 0 )
		{
			continue;
		}

		const TileDefinition& def = TileDefinition::s_definitions.at( tile.m_type );

		float x = ( float )tile.m_tileCoords.x;
		float y = ( float )tile.m_tileCoords.y;
		AABB3 bounds( Vec3( x, y, 0.f ), Vec3( x + 1.f, y + 1.f, 1.f ) );

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