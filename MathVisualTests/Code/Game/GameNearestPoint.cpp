#include "Game/GameNearestPoint.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"

//------------------------------------------------------------------------------
GameNearestPoint::GameNearestPoint(App* app)
	: Game(app)
{
}

GameNearestPoint::~GameNearestPoint()
{
	Shutdown();
}

//-----------------------------------------------------------------------------------------------
void GameNearestPoint::Startup()
{
	Game::Startup();
	AddShapeVerts();
}

//-----------------------------------------------------------------------------------------------
void GameNearestPoint::Shutdown()
{
	m_testShapes.clear();
	m_pointVerts.clear();
}

//-----------------------------------------------------------------------------------------------
void GameNearestPoint::Update( float deltaSeconds )
{	
	if ( m_isSlowMo ) // T pressed
	{
		deltaSeconds = 1.f / 600.f; // Run at 1/10th the speed
	}
	UpdatePointPosition(deltaSeconds);
	UpdateCameras( deltaSeconds );
	Game::UpdateKeyboardInput();

	if ( !m_isPaused || m_pauseAfterNextUpdate )
	{
		m_pauseAfterNextUpdate = false;
	}
}

//-----------------------------------------------------------------------------------------------
void GameNearestPoint::Render() const
{
	Rgba8 backgroundColor = Rgba8( static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 255.f ) ); // Suppresses error with conversion

	g_engine->m_render->ClearScreen( backgroundColor );

	g_engine->m_render->BeginCamera( *m_worldCamera );

	RenderShapes();
	Vertex tempPointWorldVerts[DISC_VERTS];
	for ( int vertIndex = 0; vertIndex < m_pointVerts.size(); ++vertIndex )
	{
		tempPointWorldVerts[vertIndex] = m_pointVerts[vertIndex];
	}
	TransformVertexArrayXY3D(static_cast<int>(m_pointVerts.size()), tempPointWorldVerts, .25f, 0.f, m_pointPos);
	g_engine->m_render->DrawVertexArray(DISC_VERTS, tempPointWorldVerts);

	Game::RenderGameText( GAMEMODE_NEAREST_POINT );

	g_engine->m_render->EndCamera( *m_worldCamera );
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
		GetRandomPosition( -50.f, 50.f, -50.f, 50.f ),
		GetRandomPosition( 150.f, 250.f, 150.f, 250.f ),
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

	/*Vec2 moveDirection = Vec2(0.f,0.f);
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
	}*/

	if ( g_engine->m_input->IsKeyDown( 'W' ) )
	{
		m_pointPos = m_pointPos + Vec2( 0.f, 1.f );
	}
	if ( g_engine->m_input->IsKeyDown( 'A' ) )
	{
		m_pointPos = m_pointPos + Vec2( -1.f, 0.f );
	}
	if ( g_engine->m_input->IsKeyDown( 'S' ) )
	{
		m_pointPos = m_pointPos + Vec2( 0.f, -1.f );
	}
	if ( g_engine->m_input->IsKeyDown( 'D' ) )
	{
		m_pointPos = m_pointPos + Vec2( 1.f, 0.f );
	}

	/*if ( moveDirection.GetLengthSquared() > 0.f ) {
		moveDirection.Normalize();
		m_pointPos += moveDirection * m_speed * deltaSeconds;
	}*/

	if ( g_engine->m_input->IsKeyDown( KEYCODE_LEFT_MOUSE ) )
	{
		AABB2 screenSize( m_worldCamera->GetOrthoBottomLeft(), m_worldCamera->GetOrthoTopRight() );
		Vec2 screenMouseUV = g_engine->m_window->GetNormalizedMouseUV();
		m_pointPos = screenSize.GetPointAtUV( screenMouseUV );
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