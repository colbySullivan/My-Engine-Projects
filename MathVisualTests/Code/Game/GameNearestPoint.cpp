#include "Game/GameNearestPoint.hpp"
#include "Engine/Core/VertexUtils.hpp"

//------------------------------------------------------------------------------
GameNearestPoint::GameNearestPoint(App* app)
	: Game(app)
{
	AddShapeVerts();
}

//-----------------------------------------------------------------------------------------------
void GameNearestPoint::Update( float deltaTime )
{	
	
}

//-----------------------------------------------------------------------------------------------
void GameNearestPoint::Render()
{

}

//-----------------------------------------------------------------------------------------------
void GameNearestPoint::AddShapeVerts()
{
	AddVertsForLineSegment2D(m_lineVerts, Vec2(0.f,0.f), Vec2(1.f,1.f), Vec2(1.f,1.f), Rgba8(255, 255, 255, 255));
}
