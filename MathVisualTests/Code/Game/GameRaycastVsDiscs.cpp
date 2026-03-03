#include "Game/GameRaycastVsDiscs.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/StringUtils.hpp"

//------------------------------------------------------------------------------
GameRaycastVsDiscs::GameRaycastVsDiscs( App* app )
	: Game( app )
{
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
	UpdateKeyboardPoints( deltaSeconds );
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
	for ( int Index = 0; Index < static_cast< int >( m_testShapes.size() ); Index++ )
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

	Vec2 lineStart = m_tailPos;

	for ( int raycastIndex = 0; raycastIndex < m_raycastResults.size(); ++raycastIndex )
	{
		RaycastResult2D result = m_raycastResults[raycastIndex];

		if ( !result.m_didImpact )
		{
			AddVertsForArrow2D( m_lineVerts, lineStart, result.m_impactPos, 2.f, .5f, Rgba8( 0, 255, 0, 255 ) );
			break;
		}

		AddVertsForArrow2D( m_lineVerts, lineStart, result.m_impactPos, 2.f, .5f, Rgba8( 255, 0, 0, 255 ) );

		Vec2 normalEnd = result.m_impactPos + result.m_impactNormal * 10.f;
		AddVertsForArrow2D( m_lineVerts, result.m_impactPos, normalEnd, 2.f, .5f, Rgba8( 255, 255, 0, 255 ) );

		lineStart = result.m_impactPos;
	}
	if ( m_raycastResults.size() > 0 )
	{
		AddVertsForArrow2D( m_lineVerts, m_raycastResults[0].m_impactPos, m_tipPos, 2.f, .5f, Rgba8( 120, 120, 120, 255 ) );
	}
}

//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::UpdateKeyboardPoints( float deltaSeconds )
{
	float keyboardSpeed = deltaSeconds * 100.f;

	// Tail point
	if ( g_engine->m_input->IsKeyDown( 'W' ) )
	{
		m_tailPos = m_tailPos + Vec2( 0.f, 1.f ) * keyboardSpeed;
	}
	if ( g_engine->m_input->IsKeyDown( 'A' ) )
	{
		m_tailPos = m_tailPos + Vec2( -1.f, 0.f ) * keyboardSpeed;
	}
	if ( g_engine->m_input->IsKeyDown( 'S' ) )
	{
		m_tailPos = m_tailPos + Vec2( 0.f, -1.f ) * keyboardSpeed;
	}
	if ( g_engine->m_input->IsKeyDown( 'D' ) )
	{
		m_tailPos = m_tailPos + Vec2( 1.f, 0.f ) * keyboardSpeed;
	}

	// Head point
	if ( g_engine->m_input->IsKeyDown( 'I' ) )
	{
		m_tipPos = m_tipPos + Vec2( 0.f, 1.f ) * keyboardSpeed;
	}
	if ( g_engine->m_input->IsKeyDown( 'J' ) )
	{
		m_tipPos = m_tipPos + Vec2( -1.f, 0.f ) * keyboardSpeed;
	}
	if ( g_engine->m_input->IsKeyDown( 'K' ) )
	{
		m_tipPos = m_tipPos + Vec2( 0.f, -1.f ) * keyboardSpeed;
	}
	if ( g_engine->m_input->IsKeyDown( 'L' ) )
	{
		m_tipPos = m_tipPos + Vec2( 1.f, 0.f ) * keyboardSpeed;
	}

	if ( g_engine->m_input->IsKeyDown( KEYCODE_LEFT_MOUSE ) )
	{
		AABB2 screenSize( m_worldCamera->GetOrthoBottomLeft(), m_worldCamera->GetOrthoTopRight() );
		Vec2 screenMouseUV = g_engine->m_window->GetNormalizedMouseUV();
		m_tailPos = screenSize.GetPointAtUV( screenMouseUV );
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
void GameRaycastVsDiscs::UpdateCheckDiscsRaycast() //#TODO Need to fix through vector and case where the ray starts inside a disc
{
	m_raycastResults.clear();
	if ( m_lastClosestShape )
	{
		m_lastClosestShape->ChangeColor( Rgba8( 0, 0, 255 ) );
		m_lastClosestShape = nullptr;
	}

	for ( int shapeIndex = 0; shapeIndex < m_testShapes.size() - 1; ++shapeIndex )
	{
		TestShapeDisc* shape = m_testShapes[shapeIndex];
		if ( shape )
		{
			shape->ChangeColor( Rgba8( 0, 0, 255 ) );
		}
	}

	Vec2 lineStart = m_tailPos;
	Vec2 fwdLine = m_tipPos - m_tailPos;
	float length = fwdLine.GetLength();
	fwdLine.Normalize();
	float remainingLength = length;

	int lineBounces = 0;

	while ( remainingLength > 0.f && lineBounces <= 100 )
	{
		float smallestImpactDist = 999999.f;
		TestShapeDisc* hitShape = nullptr;
		RaycastResult2D bestHit;

		for ( int shapeIndex = 0; shapeIndex < m_testShapes.size(); ++shapeIndex )
		{
			TestShapeDisc* shape = m_testShapes[shapeIndex];
			if ( shape )
			{
				RaycastResult2D raycastTestCheck = RaycastVsDisc2D( lineStart, fwdLine, remainingLength, shape->m_center, 10.f );

				if ( raycastTestCheck.m_didImpact && raycastTestCheck.m_impactDist < smallestImpactDist )
				{
					smallestImpactDist = raycastTestCheck.m_impactDist;
					bestHit = raycastTestCheck;
					hitShape = shape;
					if ( shape->IsPointInsideMe( m_tailPos ) )
					{
						remainingLength = -1.f;
						break;
					}
				}
			}
		}
		if ( !hitShape )
		{
			RaycastResult2D noHit;
			noHit.m_didImpact = false;
			noHit.m_impactPos = lineStart + fwdLine * remainingLength;
			m_raycastResults.push_back( noHit );
			break;
		}

		hitShape->ChangeColor( Rgba8( 0, 0, 150 ) );
		m_lastClosestShape = hitShape;
		m_raycastResults.push_back( bestHit );

		remainingLength -= smallestImpactDist;

		fwdLine = fwdLine.GetReflected( bestHit.m_impactNormal );

		lineStart = bestHit.m_impactPos + fwdLine * 0.001f;
		lineBounces++;
	}
}