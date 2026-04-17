#include "Game/Game2DCurves.hpp"
#include "Engine/Core/Engine.hpp"

//-----------------------------------------------------------------------------------------------
Game2DCurves::Game2DCurves( App* app )
	: Game( app )
{

}

Game2DCurves::~Game2DCurves()
{
	Shutdown();
}

void Game2DCurves::Startup()
{
	Game::Startup();
}

void Game2DCurves::Shutdown()
{

}

void Game2DCurves::Update( float deltaSeconds )
{
	UpdateCameras( deltaSeconds );
	Game::UpdateKeyboardInput();
}

void Game2DCurves::Render() const
{
	Rgba8 backgroundColor = Rgba8( static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 255.f ) ); // Suppresses error with conversion

	g_engine->m_render->ClearScreen( backgroundColor );

	g_engine->m_render->BeginCamera( *m_worldCamera );

	Game::RenderGameText( GAMEMODE_2D_CURVES );

	g_engine->m_render->EndCamera( *m_worldCamera );
}

