#include "Game/Game2DPachinkoMachine.hpp"
#include "TestShapeCapsule.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "TestShapeDisc.hpp"
#include "TestShapeOBB2.hpp"

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
	// Physics updates
	UpdateGravity( deltaSeconds );
	UpdateBounceBallsEachOther();
	UpdateBounceBallsBumpers();
	UpdateBallsWalls();

	UpdateVelocityPoints();
	UpdateMousePosition();
	UpdateSpawnBalls();
	Game::UpdateKeyboardInput();

}

//-----------------------------------------------------------------------------------------------
void Game2DPachinkoMachine::Render() const
{
	g_engine->m_render->BindTexture( nullptr );
	Rgba8 backgroundColor = Rgba8( 0, 0, 0 );
	g_engine->m_render->ClearScreen( backgroundColor );
	g_engine->m_render->BeginCamera( *m_worldCamera );

	RenderShapes();
	RenderMouseOutline();
	RenderVelocityLine();
	Game::RenderGameText( GAMEMODE_2D_PACHINKO );

	g_engine->m_render->EndCamera( *m_worldCamera );
}

//-----------------------------------------------------------------------------------------------
void Game2DPachinkoMachine::AddShapes()
{
	int numOfShapesEach = 1;

	// Capsules
	for (int Index = 0; Index < numOfShapesEach ; ++Index)
	{
		Vec2 randomPos = g_rng->GetRandom2DPosition( 10.f, m_worldCamera->GetOrthoTopRight().x - 10.f, 10.f, m_worldCamera->GetOrthoTopRight().y - 10.f );
		Vec2 randomPosOffset = Vec2( randomPos.x + g_rng->RollRandomIntInRange(-3, 3), randomPos.y + g_rng->RollRandomIntInRange(-3, 3));
		float randomRadius = g_rng->RollRandomFloatInRange( 2.f, 3.f );
		TestShape* capsule = new TestShapeCapsule(
			randomPos,
			randomPosOffset,
			randomRadius,
			Rgba8( 0, 255, 0, 255 )
		);
		m_testShapes.push_back( capsule );
	}

	// Discs
	for ( int Index = 0; Index < numOfShapesEach; ++Index )
	{
		Vec2 randomPos = g_rng->GetRandom2DPosition( 10.f, m_worldCamera->GetOrthoTopRight().x - 10.f, 10.f, m_worldCamera->GetOrthoTopRight().y - 10.f );
		float randomRadius = g_rng->RollRandomFloatInRange( 2.f, 3.f );
		TestShape* disc = new TestShapeDisc(
			randomPos,
			randomRadius,
			Rgba8( 255, 255, 255, 255 )
		);
		m_testShapes.push_back( disc );
	}

	//OBB2s
	for ( int Index = 0; Index < numOfShapesEach; ++Index )
	{
		Vec2 iBasis = Vec2( 1.f, 1.f );
		iBasis.Normalize();
		Vec2 randomPosCenter = g_rng->GetRandom2DPosition( 10.f, m_worldCamera->GetOrthoTopRight().x - 10.f, 10.f, m_worldCamera->GetOrthoTopRight().y - 10.f );
		Vec2 halfDimensions = Vec2(
			g_rng->RollRandomFloatInRange( 1.f, 5.f ),
			g_rng->RollRandomFloatInRange( 1.f, 5.f )
		);
		TestShape* obb2 = new TestShapeOBB2(
			randomPosCenter,
			iBasis,
			halfDimensions,
			Rgba8( 255, 255, 255, 255 )
		);
		m_testShapes.push_back( obb2 );
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

	for ( int Index = 0; Index < static_cast< int >( m_balls.size() ); Index++ )
	{
		TestShape* shape = m_balls[Index];
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

//-----------------------------------------------------------------------------------------------
void Game2DPachinkoMachine::UpdateSpawnBalls()
{
	if ( g_engine->m_input->WasKeyJustPressed( ' ' ) )
	{
		float randomRadius = g_rng->RollRandomFloatInRange( 0.5f, 2.f );
		TestShapeDisc* disc = new TestShapeDisc(
			m_tailPos,
			randomRadius,
			Rgba8( 0, 0, 125 * randomRadius, 255 )
		);
		disc->m_isPhysicsSimulated = true;
		TestShapeDisc* discB = dynamic_cast< TestShapeDisc* >( disc );
		Vec2 normalizedLaunch = ( m_tipPos - m_tailPos );
		normalizedLaunch.GetNormalized();
		discB->m_velocity = normalizedLaunch * 3.f;
		m_balls.push_back( disc );
	}

	if ( g_engine->m_input->IsKeyDown( 'N' ) )
	{
		float randomRadius = g_rng->RollRandomFloatInRange( 0.5f, 2.f );
		TestShapeDisc* disc = new TestShapeDisc(
			m_tailPos,
			randomRadius,
			Rgba8( 0, 0, 125 * randomRadius, 255 )
		);
		disc->m_isPhysicsSimulated = true;
		TestShapeDisc* discB = dynamic_cast< TestShapeDisc* >( disc );
		Vec2 normalizedLaunch = ( m_tipPos - m_tailPos );
		normalizedLaunch.GetNormalized();
		discB->m_velocity = normalizedLaunch * 3.f;
		m_balls.push_back( disc );
	}
}

//-----------------------------------------------------------------------------------------------
void Game2DPachinkoMachine::UpdateBounceBallsEachOther()
{
	if ( m_balls.size() < 2 )
	{
		return;
	}
	for ( int i = 0; i < static_cast< int >( m_balls.size() ); ++i )
	{
		TestShapeDisc* discA = m_balls[i];
		for ( int j = i + 1; j < static_cast< int >( m_balls.size() ); ++j )
		{
			TestShapeDisc* discB = m_balls[j];
			if ( !discB )
			{
				return;
			}
			DiscBounceOffDisc(
				discA->m_center,
				discA->m_velocity,
				discA->m_discRadius,
				m_elasticity,
				discB->m_center,
				discB->m_velocity,
				discB->m_discRadius,
				m_elasticity
			);
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Game2DPachinkoMachine::UpdateGravity( float deltaSeconds )
{
	float gravity = -50.f;

	for ( int i = 0; i < static_cast< int >( m_balls.size() ); ++i )
	{
		TestShapeDisc* disc = dynamic_cast< TestShapeDisc* >( m_balls[i] );
		if ( disc == nullptr || !m_balls[i]->m_isPhysicsSimulated )
		{
			continue;
		}
		disc->m_velocity.y += gravity * deltaSeconds;
		disc->m_center += disc->m_velocity * deltaSeconds;

		// Move ball
		disc->m_myVerts.clear();
		disc->AddVertsForMe( disc->m_myVerts );
	}

}

//-----------------------------------------------------------------------------------------------
void Game2DPachinkoMachine::UpdateVelocityPoints()
{
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

void Game2DPachinkoMachine::UpdateBallsWalls()
{
	for ( int i = 0; i < static_cast< int >( m_balls.size() ); ++i )
	{
		TestShapeDisc* disc = dynamic_cast< TestShapeDisc* >( m_balls[i] );
		if ( disc == nullptr || !m_balls[i]->m_isPhysicsSimulated )
		{
			continue;
		}

		AABB2 worldBounds = AABB2( m_worldCamera->GetOrthoBottomLeft(), m_worldCamera->GetOrthoTopRight() );
		Vec2 normal = Vec2( 0.f, 0.f );

		// Top
		if ( disc->m_center.y + disc->m_discRadius > worldBounds.m_maxs.y )
		{
			PushDiscOutOfFixedAABB2D( disc->m_center, disc->m_discRadius, AABB2( Vec2( 0.f, worldBounds.m_maxs.y ), Vec2( worldBounds.m_maxs.x, worldBounds.m_maxs.y ) ) );
			normal = Vec2( 0.f, -1.f );
		}

		// Bottom
		if ( disc->m_center.y - disc->m_discRadius < worldBounds.m_mins.y )
		{
			PushDiscOutOfFixedAABB2D( disc->m_center, disc->m_discRadius, AABB2(Vec2(0.f,0.f), Vec2(worldBounds.m_maxs.x, 0.f) ) );
			normal = Vec2( 0.f, 1.f );
		}

		// Left
		if ( disc->m_center.x - disc->m_discRadius < worldBounds.m_mins.x )
		{
			PushDiscOutOfFixedAABB2D( disc->m_center, disc->m_discRadius, AABB2( Vec2( 0.f, 0.f ), Vec2( 0.f, worldBounds.m_maxs.y ) ) );
			normal = Vec2( 1.f, 0.f );
		}

		// Right
		if ( disc->m_center.x + disc->m_discRadius > worldBounds.m_maxs.x )
		{
			PushDiscOutOfFixedAABB2D( disc->m_center, disc->m_discRadius, AABB2( Vec2( worldBounds.m_maxs.x, worldBounds.m_mins.y ), Vec2( worldBounds.m_maxs.x, worldBounds.m_maxs.y ) ) );
			normal = Vec2( -1.f, 0.f );
		}

		Vec2 VelOnNormal = DotProduct2D( disc->m_velocity, normal ) * normal;
		Vec2 VelOnT = disc->m_velocity - VelOnNormal;
		float totalElasticity = m_wallElasticity * m_elasticity;
		disc->m_velocity = VelOnT + ( -VelOnNormal * totalElasticity );
	}
	
}

//-----------------------------------------------------------------------------------------------
void Game2DPachinkoMachine::UpdateBounceBallsBumpers()
{
	for ( int i = 0; i < static_cast< int >( m_balls.size() ); ++i )
	{
		TestShapeDisc* discA = m_balls[i];

		for ( int j = 0; j < static_cast< int >( m_testShapes.size() ); ++j )
		{
			TestShapeDisc* discB = dynamic_cast< TestShapeDisc* >( m_testShapes[j] );
			if ( discB != nullptr )
			{
				DiscBounceOffDisc(
					discA->m_center,
					discA->m_velocity,
					discA->m_discRadius,
					m_elasticity,
					discB->m_center,
					discB->m_velocity,
					discB->m_discRadius,
					m_elasticity
				);
				continue;
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Game2DPachinkoMachine::RenderMouseOutline() const
{
	std::vector<Vertex> tempCircleVerts;
	AddVertsForDisc2D( tempCircleVerts, m_tailPos, 2.0f, Rgba8::WHITE );
	AddVertsForDisc2D( tempCircleVerts, m_tailPos, 1.9f, Rgba8::BLACK );
	AddVertsForDisc2D( tempCircleVerts, m_tailPos, 0.8f, Rgba8::WHITE );
	AddVertsForDisc2D( tempCircleVerts, m_tailPos, 0.7f, Rgba8::BLACK );
	g_engine->m_render->DrawVertexArray( tempCircleVerts );
}

//-----------------------------------------------------------------------------------------------
void Game2DPachinkoMachine::RenderVelocityLine() const
{
	std::vector<Vertex> tempLineVerts;
	AddVertsForArrow2D( tempLineVerts, m_tailPos, m_tipPos, 2.f, .5f, Rgba8( 0, 120, 0, 255 ) );
	g_engine->m_render->DrawVertexArray( tempLineVerts );
}
