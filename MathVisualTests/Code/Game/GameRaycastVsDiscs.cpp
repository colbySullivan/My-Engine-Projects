#include "Game/GameRaycastVsDiscs.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Camera.hpp"

//------------------------------------------------------------------------------
GameRaycastVsDiscs::GameRaycastVsDiscs( App* app )
	: Game( app )
{
	// TODO randomize this
	m_tailPos = Vec2( 0.f, 0.f );
	m_tipPos = Vec2( 150.f, 75.f );
}

GameRaycastVsDiscs::~GameRaycastVsDiscs()
{
	Shutdown();
}

//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::Startup()
{
	Game::Startup();
	AddShapeVerts();
}

//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::Shutdown()
{
	m_testShapes.clear();
}

//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::Update( [[maybe_unused]] float deltaSeconds )
{
	UpdateCameras( deltaSeconds );
	if ( m_isSlowMo ) // T pressed
	{
		deltaSeconds = 1.f / 600.f; // Run at 1/10th the speed
	}

	UpdateCameras( deltaSeconds );
	Game::UpdateKeyboardInput();

	if ( !m_isPaused || m_pauseAfterNextUpdate )
	{
		m_pauseAfterNextUpdate = false;
	}
	UpdateCheckDiscsRaycast();
	UpdateLine();
	UpdateKeyboardPoints();
}

//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::Render() const
{
	Rgba8 backgroundColor = Rgba8( static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 255.f ) ); // Suppresses error with conversion

	g_engine->m_render->ClearScreen( backgroundColor );

	g_engine->m_render->BeginCamera( *m_worldCamera );

	RenderShapes();
	g_engine->m_render->DrawVertexArray( m_lineVerts );
	Game::RenderGameText( GAMEMODE_RAYCAST_VS_DISCS );

	g_engine->m_render->EndCamera( *m_worldCamera );
}

//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::AddShapeVerts()
{
	for ( int discNumber = 0; discNumber < m_numberOfDiscs; ++discNumber )
	{
		TestShapeDisc* disc = new TestShapeDisc(
			GetRandomPosition( 10.f, 190.f, 10.f, 90.f ),
			10.0f,
			Rgba8( 0, 0, 255, 255 )
		);
		m_testShapes.push_back( disc );
	}
}

//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::RenderShapes() const
{
	for ( int Index = 0; Index < m_testShapes.size(); Index++ )
	{
		TestShapeDisc* shape = m_testShapes[Index];
		if ( shape != nullptr )
		{
			shape->Render();
		}
	}
}

//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::UpdateLine()
{
	m_lineVerts.clear();

	if ( m_raycastResult.m_didImpact )
	{
		AddVertsForArrow2D( m_lineVerts, m_tailPos, m_raycastResult.m_impactPos, 2.f, .5f, Rgba8( 255, 255, 255, 255 ) );

		float normalLength = 10.f;
		Vec2 normalImpactExtended = m_raycastResult.m_impactPos + ( m_raycastResult.m_impactNormal * normalLength );
		AddVertsForArrow2D( m_lineVerts, m_raycastResult.m_impactPos, normalImpactExtended, 2.f, .5f, Rgba8( 0, 255, 0, 255 ) );
		AddVertsForArrow2D( m_lineVerts, m_raycastResult.m_impactPos, m_tipPos, 2.f, .5f, Rgba8( 255, 0, 0, 255 ) );
	}
	else
	{
		AddVertsForArrow2D( m_lineVerts, m_tailPos, m_tipPos, 2.f, .5f, Rgba8( 255, 255, 255, 255 ) );
	}
}

//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::UpdateKeyboardPoints()
{
	// Tail point
	if ( g_engine->m_input->IsKeyDown( 'W' ) )
	{
		m_tailPos = m_tailPos + Vec2(0.f,1.f);
	}
	if ( g_engine->m_input->IsKeyDown( 'A' ) )
	{
		m_tailPos = m_tailPos + Vec2( -1.f, 0.f );
	}
	if ( g_engine->m_input->IsKeyDown( 'S' ) )
	{
		m_tailPos = m_tailPos + Vec2( 0.f, -1.f );
	}
	if ( g_engine->m_input->IsKeyDown( 'D' ) )
	{
		m_tailPos = m_tailPos + Vec2( 1.f, 0.f );
	}

	// Head point
	if ( g_engine->m_input->IsKeyDown( 'I' ) )
	{
		m_tipPos = m_tipPos + Vec2( 0.f, 1.f );
	}
	if ( g_engine->m_input->IsKeyDown( 'J' ) )
	{
		m_tipPos = m_tipPos + Vec2( -1.f, 0.f );
	}
	if ( g_engine->m_input->IsKeyDown( 'K' ) )
	{
		m_tipPos = m_tipPos + Vec2( 0.f, -1.f );
	}
	if ( g_engine->m_input->IsKeyDown( 'L' ) )
	{
		m_tipPos = m_tipPos + Vec2( 1.f, 0.f );
	}

	if ( g_engine->m_input->IsKeyDown( KEYCODE_LEFT_MOUSE ) )
	{
		AABB2 screenSize( m_worldCamera->GetOrthoBottomLeft(), m_worldCamera->GetOrthoTopRight() );
		Vec2 screenMouseUV = g_engine->m_window->GetNormalizedMouseUV();
		m_tailPos = screenSize.GetPointAtUV(screenMouseUV);
	}

	if ( g_engine->m_input->IsKeyDown( KEYCODE_RIGHT_MOUSE ) )
	{
		AABB2 screenSize( m_worldCamera->GetOrthoBottomLeft(), m_worldCamera->GetOrthoTopRight() );
		Vec2 screenMouseUV = g_engine->m_window->GetNormalizedMouseUV();
		m_tipPos = screenSize.GetPointAtUV( screenMouseUV );
	}
}

//-----------------------------------------------------------------------------------------------
Vec2 GameRaycastVsDiscs::GetRandomPosition( float minX, float maxX, float minY, float maxY )
{
	return Vec2( g_rng->RollRandomFloatInRange( minX, maxX ), g_rng->RollRandomFloatInRange( minY, maxY ) );
}

//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::UpdateCheckDiscsRaycast()
{
	float smallestImpactDist = 99999.f;
	m_raycastResult.m_impactDist = 0.f;
	m_raycastResult.m_didImpact = false;
	Vec2 fwdLine = m_tipPos - m_tailPos;
	float length = fwdLine.GetLength();
	fwdLine.Normalize();

	for ( int Index = 0; Index < m_testShapes.size(); Index++ )
	{
		TestShapeDisc* shape = m_testShapes[Index];
		if ( shape != nullptr )
		{
			RaycastResult2D raycastTestCheck = RaycastVsDisc2D( m_tailPos, fwdLine, length, shape->m_center, 10.f );
			if ( raycastTestCheck.m_didImpact == true && raycastTestCheck.m_impactDist < smallestImpactDist )
			{
				smallestImpactDist = raycastTestCheck.m_impactDist;
				m_raycastResult = raycastTestCheck;
			}
		}
	}
}
