#include "Game/Game2DPachinkoMachine.hpp"
#include "TestShapeCapsule.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

RandomNumberGenerator* g_rng;

//-----------------------------------------------------------------------------------------------
Game2DPachinkoMachine::Game2DPachinkoMachine( App* app )
	: Game( app )
{

}

//-----------------------------------------------------------------------------------------------
Game2DPachinkoMachine::~Game2DPachinkoMachine()
{

}

//-----------------------------------------------------------------------------------------------
void Game2DPachinkoMachine::Startup()
{
	Game::Startup();
	m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );
	AddShapes();
}

//-----------------------------------------------------------------------------------------------
void Game2DPachinkoMachine::Shutdown()
{
	m_testShapes.clear();
}

//-----------------------------------------------------------------------------------------------
void Game2DPachinkoMachine::Update( float deltaSeconds )
{
	Game::UpdateKeyboardInput();
	UpdateMousePosition();
}

//-----------------------------------------------------------------------------------------------
void Game2DPachinkoMachine::Render() const
{
	g_engine->m_render->BindTexture( nullptr );

	Rgba8 backgroundColor = Rgba8( static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 255.f ) ); // Suppresses error with conversion

	g_engine->m_render->ClearScreen( backgroundColor );

	g_engine->m_render->BeginCamera( *m_worldCamera );

	RenderShapes();

	Game::RenderGameText( GAMEMODE_2D_PACHINKO );

	g_engine->m_render->EndCamera( *m_worldCamera );
}

//-----------------------------------------------------------------------------------------------
void Game2DPachinkoMachine::AddShapes()
{
	for (int Index = 0; Index < 20 ; ++Index)
	{
		Vec2 randomPos = g_rng->GetRandom2DPosition( 10.f, m_worldCamera->GetOrthoTopRight().x - 10.f, 10.f, m_worldCamera->GetOrthoTopRight().y - 10.f );
		Vec2 randomPosOffset = Vec2( randomPos.x + g_rng->RollRandomIntInRange(-3, 3), randomPos.y + g_rng->RollRandomIntInRange(-3, 3));
		TestShape* capsule = new TestShapeCapsule(
			randomPos,
			randomPosOffset,
			g_rng->RollRandomFloatInRange( 2.f, 3.f ),
			Rgba8( 0, 255, 0, 255 )
		);
		m_testShapes.push_back( capsule );
	}
}

//-----------------------------------------------------------------------------------------------
void Game2DPachinkoMachine::RenderShapes() const
{
	for ( int Index = 0; Index < static_cast< int >( m_testShapes.size() ); Index++ )
	{
		TestShape* shape = m_testShapes[Index];
		if ( shape != nullptr )
		{
			shape->Render();
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Game2DPachinkoMachine::UpdateMousePosition()
{
	AABB2 screenSize( m_worldCamera->GetOrthoBottomLeft(), m_worldCamera->GetOrthoTopRight() );
	Vec2 screenMouseUV = g_engine->m_window->GetNormalizedMouseUV();
	m_pointPos = screenSize.GetPointAtUV( screenMouseUV );
}
