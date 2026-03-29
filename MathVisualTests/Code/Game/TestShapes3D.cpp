#include "Game/TestShapes3D.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "TestShapeSphere.hpp"
#include "TestShapeAABB3.hpp"

//-----------------------------------------------------------------------------------------------
TestShapes3D::TestShapes3D( App* app )
	: Game(app)
{
	m_player = new Player( this );
	m_player->m_position = Vec3( 0.f, 0.f, 1.f );
	m_modelTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/Test_StbiFlippedAndOpenGL.png" );
}

//-----------------------------------------------------------------------------------------------
TestShapes3D::~TestShapes3D()
{

}

void TestShapes3D::Startup()
{
	Game::Startup();
	DebugAddWorldBasis( m_player->GetModelToWorldTransform(), 0.f );
	float halfHeight = 0.5f;
	Mat44 toWorld = m_player->GetModelToWorldTransform();
	Vec3 upVector = toWorld.GetKBasis3D();
	Vec3 startPos = m_player->m_position - ( upVector * halfHeight );
	Vec3 endPos = m_player->m_position + ( upVector * halfHeight );

	TestShapeSphere* testShape = new TestShapeSphere( Vec3( 10.f, 10.f, 0.f ), 1.0f, 32, 32 );
	m_testShapes.push_back( testShape );

	TestShapeAABB3* testShape2 = new TestShapeAABB3( AABB3( Vec3( -1.f, -1.f, -1.f ), Vec3( 1.f, 1.f, 1.f ) ) );
	m_testShapes.push_back( testShape2 );
}

void TestShapes3D::Shutdown()
{

}

void TestShapes3D::Update( float deltaSeconds )
{
	m_player->Update( ( float )g_engine->m_systemClock->GetDeltaSeconds() );


	Game::UpdateKeyboardInput();

}

void TestShapes3D::Render() const
{
	g_engine->m_render->BindTexture( nullptr );
	g_engine->m_render->BeginCamera( *m_player->m_worldCamera );
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	Rgba8 backgroundColor = Rgba8( static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ) ); // Suppresses error with conversion
	g_engine->m_render->ClearScreen( backgroundColor );
	DebugRenderWorld( *m_player->m_worldCamera );
	for ( int shapeIndex = 0; shapeIndex < static_cast<int>( m_testShapes.size() ); ++shapeIndex )
	{
		TestShape3D* shape = m_testShapes[shapeIndex];
		if ( shape != nullptr )
		{
			shape->RenderWithTexture( m_modelTexture );
		}
	}
	g_engine->m_render->EndCamera( *m_player->m_worldCamera );
	g_engine->m_render->SetBlendMode( BlendMode::OPAQUE );
	g_engine->m_render->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
	g_engine->m_render->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
}

