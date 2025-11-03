#include "Game/GameNearestPoint.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Camera.hpp"

//------------------------------------------------------------------------------
GameNearestPoint::GameNearestPoint(App* app)
	: Game(app)
{
}

GameNearestPoint::~GameNearestPoint()
{
	for ( int Index = 0; Index < m_testShapes.size(); Index++ )
	{
		if ( m_testShapes[Index] != nullptr )
		{
			delete m_testShapes[Index];
			m_testShapes[Index] = nullptr;
		}
	}
	m_testShapes.clear();
}

//-----------------------------------------------------------------------------------------------
void GameNearestPoint::Startup()
{
	AddShapeVerts();
}

//-----------------------------------------------------------------------------------------------
void GameNearestPoint::Update( float deltaSeconds )
{	
	UpdatePointPosition(deltaSeconds);
}

//-----------------------------------------------------------------------------------------------
void GameNearestPoint::Render() const
{
	RenderShapes();
	
	Vertex tempPointWorldVerts[DISC_VERTS];
	for ( int vertIndex = 0; vertIndex < m_pointVerts.size(); ++vertIndex )
	{
		tempPointWorldVerts[vertIndex] = m_pointVerts[vertIndex];
	}
	TransformVertexArrayXY3D(static_cast<int>(m_pointVerts.size()), tempPointWorldVerts, .25f, 0.f, m_pointPos);
	g_engine->m_render->DrawVertexArray(DISC_VERTS, tempPointWorldVerts);
}

//-----------------------------------------------------------------------------------------------
void GameNearestPoint::AddShapeVerts()
{
	AddVertsForDisc2D( m_pointVerts, Vec2( 0.f, 0.f ), 1.0f, Rgba8( 255, 255, 255, 255 ) );

	TestShape* segLine = new TestShapeLine(
		GetRandomPosition( 10.f, 190.f, 10.f, 90.f ),
		GetRandomPosition( 10.f, 190.f, 10.f, 90.f ),
		Vec2( 0.5f, 0.5f ),
		Rgba8( 255, 255, 255, 255 )
	);
	m_testShapes.push_back( segLine );

	TestShape* infiniteLine = new TestShapeLine(
		GetRandomPosition( 10.f, 90.f, 10.f, 50.f ),
		GetRandomPosition( 110.f, 190.f, 50.f, 90.f ),
		Vec2( 0.5f, 0.5f ),
		Rgba8( 255, 255, 255, 255 )
	);
	m_testShapes.push_back( infiniteLine );

	Vec2 pointA = GetRandomPosition( 10.f, 180.f, 10.f, 80.f );
	TestShape* triangle = new TestShapeTriangle(
		pointA,
		pointA - GetRandomPosition( 1.f, 20.f, 1.f, 20.f ),
		pointA + GetRandomPosition( 1.f, 20.f, 1.f, 20.f ),
		Rgba8( 255, 255, 255, 255 )
	);
	m_testShapes.push_back( triangle );

	TestShape* disc = new TestShapeDisc(
		GetRandomPosition( 10.f, 190.f, 10.f, 90.f ),
		10.0f,
		Rgba8( 255, 255, 255, 255 )
	);
	m_testShapes.push_back( disc );

	Vec2 iBasis = Vec2( 1.f, 1.f );
	iBasis.Normalize();
	Vec2 center = GetRandomPosition( 30.f, 170.f, 30.f, 70.f );
	Vec2 halfDimensions = Vec2(
		g_rng->RollRandomFloatInRange( 5.f, 15.f ),
		g_rng->RollRandomFloatInRange( 5.f, 15.f )
	);
	TestShape* obb2 = new TestShapeOBB2(
		center,
		iBasis,
		halfDimensions,
		Rgba8( 255, 255, 255, 255 )
	);
	m_testShapes.push_back( obb2 );

	Vec2 aabbMin = GetRandomPosition( 10.f, 150.f, 10.f, 70.f );
	Vec2 aabbMax = Vec2(
		aabbMin.x + g_rng->RollRandomFloatInRange( 10.f, 40.f ),
		aabbMin.y + g_rng->RollRandomFloatInRange( 10.f, 30.f )
	);
	TestShape* abb2 = new TestShapeAABB2(
		aabbMin,
		aabbMax,
		Rgba8( 255, 255, 255, 255 )
	);
	m_testShapes.push_back( abb2 );

	TestShape* capsule = new TestShapeCapsule(
		GetRandomPosition( 10.f, 140.f, 10.f, 70.f ),
		GetRandomPosition( 60.f, 190.f, 30.f, 90.f ),
		g_rng->RollRandomFloatInRange( 3.f, 8.f ),
		Rgba8( 255, 255, 255, 255 )
	);
	m_testShapes.push_back( capsule );
}

//-----------------------------------------------------------------------------------------------
void GameNearestPoint::UpdatePointPosition( float deltaSeconds )
{

	Vec2 moveDirection = Vec2(0.f,0.f);
	if ( g_engine->m_input->IsKeyDown( 'W' ) ) {
		moveDirection.y += 1.f;
	}
	if ( g_engine->m_input->IsKeyDown( 'S' ) ) {
		moveDirection.y = -1.f;
	}
	if ( g_engine->m_input->IsKeyDown( 'A' ) ) {
		moveDirection.x = -1.f;
	}
	if ( g_engine->m_input->IsKeyDown( 'D' ) ) {
		moveDirection.x = 1.f;
	}

	if ( moveDirection.GetLengthSquared() > 0.f ) {
		moveDirection.Normalize();
		m_pointPos += moveDirection * m_speed * deltaSeconds;
	}
}

//-----------------------------------------------------------------------------------------------
void GameNearestPoint::RenderShapes() const
{
	for (int Index = 0; Index < m_testShapes.size() ; Index++)
	{
		TestShape* shape = m_testShapes[Index];
		if ( shape != nullptr )
		{
			if (shape->IsPointInsideMe(m_pointPos))
			{
				shape->ChangeColor(Rgba8(0, 0, 255, 255));
			}
			else
			{
				shape->ChangeColor(Rgba8(0, 0, 255, 150));
			}
			shape->GetClosestPoint(m_pointPos);
			shape->Render();
		}
	}
}

//-----------------------------------------------------------------------------------------------
Vec2 GameNearestPoint::GetRandomPosition( float minX, float maxX, float minY, float maxY )
{
	return Vec2( g_rng->RollRandomFloatInRange( minX, maxX ), g_rng->RollRandomFloatInRange( minY, maxY ) );
}
