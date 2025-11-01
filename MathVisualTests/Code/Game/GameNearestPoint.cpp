#include "Game/GameNearestPoint.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Camera.hpp"

//------------------------------------------------------------------------------
GameNearestPoint::GameNearestPoint(App* app)
	: Game(app)
{
	AddShapeVerts();
}

GameNearestPoint::~GameNearestPoint()
{

}

//-----------------------------------------------------------------------------------------------
void GameNearestPoint::Update( float deltaTime )
{	
	Game::UpdateCameras(deltaTime);
	UpdatePointPosition(deltaTime);
}

//-----------------------------------------------------------------------------------------------
void GameNearestPoint::Render() const
{
	g_engine->m_render->BeginCamera( *m_worldCamera );
	Rgba8 backgroundColor = Rgba8( static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 255.f ) ); // Suppresses error with conversion
	g_engine->m_render->ClearScreen( backgroundColor );

	// TODO make this a disc
	//-----------------------------------------------------------------------------------------------
	Vertex tempPointWorldVerts[6];
	for ( int vertIndex = 0; vertIndex < m_pointVerts.size(); ++vertIndex )
	{
		tempPointWorldVerts[vertIndex] = m_pointVerts[vertIndex];
	}
	TransformVertexArrayXY3D(m_pointVerts.size(), tempPointWorldVerts, .5f, 0.f, m_pointPos);
	g_engine->m_render->DrawVertexArray(6, tempPointWorldVerts);
	//-----------------------------------------------------------------------------------------------
	RenderShapes();

	g_engine->m_render->EndCamera( *m_worldCamera );
}

//-----------------------------------------------------------------------------------------------
void GameNearestPoint::AddShapeVerts()
{
	TestShape* segLine = new TestShapeLine( Vec2( 10.f, 10.f ), Vec2( 50.f, 60.f ), Vec2( 0.5f, 0.5f ), Rgba8( 255, 255, 255, 255 ) );
	m_testShapes.push_back(segLine);

	TestShape* infiniteLine = new TestShapeLine(Vec2(200.f, 0.f), Vec2(50.f, 600.f), Vec2(0.5f, 0.5f), Rgba8(255, 255, 255, 255));
	m_testShapes.push_back(infiniteLine);

	// Player moving point shape TODO make disc and push to m_testShapes
	AABB2 pointAABB2( 0.f, 0.f, 1.f, 1.f );
	AddVertsForAABB2D( m_pointVerts, pointAABB2, Rgba8( 0, 255, 0, 255 ) );

	TestShape* triangle = new TestShapeTriangle(Vec2( 40.f, 40.f ), Vec2( 40.f, 10.f ), Vec2( 60.f, 60.f ), Rgba8( 255, 255, 255, 255 ));
	m_testShapes.push_back(triangle);


	Vec2 iBasis = Vec2( 1.f, 1.f );
	iBasis.Normalize();
	TestShape* obb2 = new TestShapeOBB2( Vec2( 70.f, 30.f ), iBasis, Vec2( 10.f, 5.f ), Rgba8( 255, 255, 255, 255 ) );
	m_testShapes.push_back( obb2 );
}

//-----------------------------------------------------------------------------------------------
void GameNearestPoint::UpdatePointPosition( float deltaTime )
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
		m_pointPos += moveDirection * m_speed * deltaTime;
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
				shape->ChangeColor(Rgba8(255, 255, 255, 255));
			}
			shape->GetClosestPoint(m_pointPos);
			shape->Render();
		}
	}
}

