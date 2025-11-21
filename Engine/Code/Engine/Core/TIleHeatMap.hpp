#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include <vector>

class TileHeatMap
{
public:
	TileHeatMap(  IntVec2 const& dimensions );
	~TileHeatMap();

	float Get( IntVec2 coordinates );
	void Set( IntVec2 coordinates );
	void Add( IntVec2 coordinates );
private:
	void SetAllValues( float resetValue );
	void AddVertsForDebugDraw( std::vector<Vertex>& verts, AABB2 totalBounds, FloatRange valueRange=FloatRange(0.f,1.f),             
		Rgba8 lowColor=Rgba8(0,0,0,100), Rgba8 highColor=Rgba8(255,255,255,100), float specialValue=999999.f, Rgba8 specialColor=Rgba8(255,0,255) );

	IntVec2			m_demensions;
	float*			m_values;
};