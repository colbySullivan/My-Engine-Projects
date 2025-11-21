#include "Engine/Core/TileHeatMap.hpp"

//-----------------------------------------------------------------------------------------------
TileHeatMap::TileHeatMap(  IntVec2 const& dimensions )
	: m_demensions( dimensions )
{
}

//-----------------------------------------------------------------------------------------------
TileHeatMap::~TileHeatMap()
{
	delete[] m_values;
}

//-----------------------------------------------------------------------------------------------
float TileHeatMap::Get( IntVec2 coordinates )
{
	return 255.f;
}

//-----------------------------------------------------------------------------------------------
void TileHeatMap::Set( IntVec2 coordinates )
{

}

//-----------------------------------------------------------------------------------------------
void TileHeatMap::Add( IntVec2 coordinates )
{

}

//-----------------------------------------------------------------------------------------------
void TileHeatMap::SetAllValues( float resetValue )
{

}

//-----------------------------------------------------------------------------------------------
void TileHeatMap::AddVertsForDebugDraw( std::vector<Vertex>& verts, AABB2 totalBounds, FloatRange valueRange, Rgba8 lowColor, Rgba8 highColor, float specialValue, Rgba8 specialColor )
{

}