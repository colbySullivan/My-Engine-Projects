#include "Game/TestShapes3D.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/DebugRender.hpp"

//-----------------------------------------------------------------------------------------------
TestShape3D::TestShape3D( App* app )
	: Game(app)
{
	m_player = new Player( this );
	m_player->m_position = Vec3( 0.f, 0.f, 1.f );
}

//-----------------------------------------------------------------------------------------------
TestShape3D::~TestShape3D()
{

}

void TestShape3D::Startup()
{
	Game::Startup();
}

void TestShape3D::Shutdown()
{

}

void TestShape3D::Update( float deltaSeconds )
{
	m_player->Update( ( float )g_engine->m_systemClock->GetDeltaSeconds() );


	if ( g_engine->m_input->WasKeyJustPressed( '1' ) )
	{
		Mat44 toWorld = m_player->GetModelToWorldTransform();
		Vec3 endPos = m_player->m_position + toWorld.GetIBasis3D() * 20.f;
		DebugAddWorldCylinder( m_player->m_position, endPos, 0.0625f, 10.f, Rgba8( 255, 255, 0 ), Rgba8( 255, 255, 0 ), DebugRenderMode::X_RAY );
	}

	if ( g_engine->m_input->IsKeyDown( '2' ) )
	{
		Vec3 playerPos = m_player->m_position;
		DebugAddWorldSphere( Vec3( playerPos.x, playerPos.y, 0.f ), 0.1f, 60.f, Rgba8( 150, 75, 0 ), Rgba8( 150, 75, 0 ), DebugRenderMode::USE_DEPTH );
	}

	if ( g_engine->m_input->WasKeyJustPressed( '3' ) )
	{
		Vec3 forward = m_player->m_orientation.GetForwardDir_IFwd_JLeft_KUp();
		Vec3 spawnPos = m_player->m_position + forward * 2.f;
		DebugAddWorldWireSphere( spawnPos, 1.f, 5.f, Rgba8( 0, 255, 0 ), Rgba8( 255, 0, 0 ), DebugRenderMode::USE_DEPTH );
	}

	if ( g_engine->m_input->WasKeyJustPressed( '4' ) )
	{
		DebugAddBasis( m_player->GetModelToWorldTransform(), 5.f, 1.f, 0.1f, 1.f, 1.f, DebugRenderMode::USE_DEPTH );
	}

	if ( g_engine->m_input->WasKeyJustPressed( '5' ) )
	{
		Vec3 forward = m_player->m_orientation.GetForwardDir_IFwd_JLeft_KUp();
		Vec3 spawnPos = m_player->m_position + forward * 2.f;
		std::string posOrientText = Stringf( "Position: %5.2f, %5.2f, %5.2f Orientation: %5.2f, %5.2f, %5.2f", m_player->m_position.x, m_player->m_position.y, m_player->m_position.z, m_player->m_orientation.m_pitchDegrees, m_player->m_orientation.m_yawDegrees, m_player->m_orientation.m_rollDegrees );
		DebugAddWorldBillboardText( posOrientText, spawnPos, 0.125f, Vec2( 0.5f, 0.5f ), 10.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 0, 0 ) );
	}

	if ( g_engine->m_input->WasKeyJustPressed( '6' ) )
	{
		float halfHeight = 0.5f;
		Mat44 toWorld = m_player->GetModelToWorldTransform();
		Vec3 upVector = toWorld.GetKBasis3D();
		Vec3 startPos = m_player->m_position - ( upVector * halfHeight );
		Vec3 endPos = m_player->m_position + ( upVector * halfHeight );
		DebugAddWorldWireCylinder( startPos, endPos, 0.5f, 10.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 0, 0 ), DebugRenderMode::USE_DEPTH );
	}

	if ( g_engine->m_input->WasKeyJustPressed( '7' ) )
	{
		std::string hudText = Stringf( "Camera orientation: %5.2f, %5.2f, %5.2f", m_player->m_orientation.m_pitchDegrees, m_player->m_orientation.m_yawDegrees, m_player->m_orientation.m_rollDegrees );
		DebugAddMessage( hudText, 5.f, Rgba8( 255, 255, 255 ), Rgba8( 255, 0, 0 ) );
	}
}

void TestShape3D::Render() const
{
	g_engine->m_render->BindTexture( nullptr );
	g_engine->m_render->BeginCamera( *m_player->m_worldCamera );
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	Rgba8 backgroundColor = Rgba8( static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ) ); // Suppresses error with conversion
	g_engine->m_render->ClearScreen( backgroundColor );
	DebugRenderWorld( *m_player->m_worldCamera );
	g_engine->m_render->EndCamera( *m_player->m_worldCamera );
	g_engine->m_render->SetBlendMode( BlendMode::OPAQUE );
	g_engine->m_render->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
	g_engine->m_render->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
}

