#include "Engine/Core/HeatMaps.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"

//-----------------------------------------------------------------------------------------------
TileHeatMap::TileHeatMap(  IntVec2 const& dimensions )
	: m_dimensions( dimensions )
{
	m_values = new float[m_dimensions.x * m_dimensions.y];
	m_mapSize = m_dimensions.x * m_dimensions.y;
}

//-----------------------------------------------------------------------------------------------
TileHeatMap::~TileHeatMap()
{
	delete[] m_values;
}

//-----------------------------------------------------------------------------------------------
float TileHeatMap::Get( IntVec2 coordinates ) const
{
	return m_values[ GetTileIndexForCoords( coordinates ) ];
}

//-----------------------------------------------------------------------------------------------
int TileHeatMap::GetMapSize() const
{
 return m_mapSize;
}

//-----------------------------------------------------------------------------------------------
void TileHeatMap::Set( IntVec2 coordinates, float value )
{
	m_values[ GetTileIndexForCoords( coordinates ) ] = value;
}

//-----------------------------------------------------------------------------------------------
void TileHeatMap::Add( IntVec2 coordinates, float addValue )
{
	m_values[ GetTileIndexForCoords( coordinates ) ] += addValue;
}

//-----------------------------------------------------------------------------------------------
void TileHeatMap::SetAllValues( float resetValue )
{
	for (int floatIndex = 0; floatIndex < m_mapSize ; ++floatIndex)
	{
		m_values[floatIndex] = resetValue;
	}
}

//-----------------------------------------------------------------------------------------------
void TileHeatMap::AddVertsForDebugDraw( std::vector<Vertex>& verts, AABB2 totalBounds, FloatRange valueRange, Rgba8 lowColor, Rgba8 highColor, float specialValue, Rgba8 specialColor )
{
	float sizeX = ( totalBounds.m_maxs.x - totalBounds.m_mins.x ) / m_dimensions.x;
	float sizeY = ( totalBounds.m_maxs.y - totalBounds.m_mins.y ) / m_dimensions.y;

	for ( int tileIndex = 0; tileIndex < m_mapSize; ++tileIndex )
	{
		int x = tileIndex % m_dimensions.x;
		int y = tileIndex / m_dimensions.x;

		Vec2 mins = totalBounds.m_mins + Vec2( x * sizeX, y * sizeY );
		Vec2 maxs = mins + Vec2( sizeX, sizeY );

		float value = m_values[tileIndex];
		Rgba8 color;

		if ( value == specialValue ) 
		{
			color = specialColor;
		}
		else
		{
			float clampedValue = RangeMapClamped( value, valueRange.m_min, valueRange.m_max, 0.f, 1.f );
			color = Interpolate( lowColor, highColor, clampedValue );
		}

		AddVertsForTriangle2D( verts, mins, Vec2( maxs.x, mins.y ), maxs, color );
		AddVertsForTriangle2D( verts, mins, maxs, Vec2( mins.x, maxs.y ), color );
	}
}

//-----------------------------------------------------------------------------------------------
int TileHeatMap::GetTileIndexForCoords( IntVec2 coordinates ) const
{
	return ( coordinates.y * m_dimensions.x ) + coordinates.x;
}
