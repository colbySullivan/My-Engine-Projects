#include "Game/Game2DPachinkoMachine.hpp"
#include "TestShapeCapsule.hpp"
#include "Engine/Core/Engine.hpp"
#include "TestShapeDisc.hpp"
#include "TestShapeOBB2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"


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
	ClearShapes();
}

//-----------------------------------------------------------------------------------------------
void Game2DPachinkoMachine::Update( float deltaSeconds )
{
	if ( g_engine->m_input->IsKeyDown( 'T' ) )
	{
		deltaSeconds *= 0.05f;
	}
	if ( m_isFixedTimeStep )
	{
		m_timeOwed += deltaSeconds;
		while ( m_timeOwed >= m_fixedTimeStep )
		{
			m_timeOwed -= m_fixedTimeStep;
			UpdatePhysics( m_fixedTimeStep );
		}
	}
	else
	{
		UpdatePhysics( deltaSeconds );
	}

	UpdateVelocityPoints();
	UpdateMousePosition();
	UpdateSpawnBalls();
	UpdateGameModeKeyboardInput();
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
	RenderInstructionText();
	Game::RenderGameText( GAMEMODE_2D_PACHINKO );

	g_engine->m_render->EndCamera( *m_worldCamera );
}

//-----------------------------------------------------------------------------------------------
void Game2DPachinkoMachine::AddShapes()
{
	int numOfShapesEach = 10;

	// Capsules
	for (int Index = 0; Index < numOfShapesEach ; ++Index)
	{
		Vec2 randomPos = g_rng->GetRandom2DPosition( 10.f, m_worldCamera->GetOrthoTopRight().x - 10.f, 10.f, m_worldCamera->GetOrthoTopRight().y - 10.f );
		Vec2 randomPosOffset = Vec2( randomPos.x + g_rng->RollRandomIntInRange(-3, 3), randomPos.y + g_rng->RollRandomIntInRange(-3, 3));
		float randomRadius = g_rng->RollRandomFloatInRange( 2.f, 3.f );
		float randomElasticity = g_rng->RollRandomFloatInRange( 0.1f, 0.9f );
		TestShape* capsule = new TestShapeCapsule(
			randomPos,
			randomPosOffset,
			randomRadius,
			Rgba8( (unsigned char)(255 - ( randomElasticity * 255.f )), (unsigned char)(randomElasticity * 255.f), 0)
		);
		capsule->m_elasticity = randomElasticity;
		m_testShapes.push_back( capsule );
	}

	// Discs
	for ( int Index = 0; Index < numOfShapesEach; ++Index )
	{
		Vec2 randomPos = g_rng->GetRandom2DPosition( 10.f, m_worldCamera->GetOrthoTopRight().x - 10.f, 10.f, m_worldCamera->GetOrthoTopRight().y - 10.f );
		float randomElasticity = g_rng->RollRandomFloatInRange( 0.1f, 0.99f );
		float randomRadius = g_rng->RollRandomFloatInRange( 2.f, 3.f );
		TestShape* disc = new TestShapeDisc(
			randomPos,
			randomRadius,
			Rgba8( ( unsigned char )( 255 - ( randomElasticity * 255.f ) ), ( unsigned char )( randomElasticity * 255.f ), 0 )
		);
		disc->m_elasticity = randomElasticity;
		m_testShapes.push_back( disc );
	}

	//OBB2s
	for ( int Index = 0; Index < numOfShapesEach; ++Index )
	{
		Vec2 iBasis = Vec2( 1.f, 1.f );
		float randomElasticity = g_rng->RollRandomFloatInRange( 0.1f, 0.99f );
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
			Rgba8( ( unsigned char )( 255 - ( randomElasticity * 255.f ) ), ( unsigned char )( randomElasticity * 255.f ), 0 )
		);
		obb2->m_elasticity = randomElasticity;
		m_testShapes.push_back( obb2 );
	}
}

//-----------------------------------------------------------------------------------------------
void Game2DPachinkoMachine::ClearShapes()
{
	for ( int i = 0; i < static_cast< int >( m_balls.size() ); ++i )
	{
		delete m_balls[i];
	}
	m_balls.clear();

	for ( int i = 0; i < static_cast< int >( m_testShapes.size() ); ++i )
	{
		delete m_testShapes[i];
	}
	m_testShapes.clear();

	m_ballsSpawned = 0;
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
			Rgba8( ( unsigned char )(255 * randomRadius), ( unsigned char )(255 * randomRadius), ( unsigned char )(200 + ( 25 * randomRadius ) ) )
		);
		disc->m_isPhysicsSimulated = true;
		TestShapeDisc* discB = dynamic_cast< TestShapeDisc* >( disc );
		Vec2 launchVelocity = ( m_tipPos - m_tailPos );
		discB->m_velocity = launchVelocity * 3.f;
		m_ballsSpawned++;
		m_balls.push_back( disc );
	}

	if ( g_engine->m_input->IsKeyDown( 'N' ) )
	{
		float randomRadius = g_rng->RollRandomFloatInRange( 0.5f, 2.f );
		TestShapeDisc* disc = new TestShapeDisc(
			m_tailPos,
			randomRadius,
			Rgba8( ( unsigned char )(255 * randomRadius), ( unsigned char )(255 * randomRadius), ( unsigned char )(200 + ( 25 * randomRadius ) ) )
		);
		disc->m_isPhysicsSimulated = true;
		TestShapeDisc* discB = dynamic_cast< TestShapeDisc* >( disc );
		Vec2 launchVelocity = ( m_tipPos - m_tailPos );
		discB->m_velocity = launchVelocity * 3.f;
		m_ballsSpawned++;
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

		if ( m_isBottomWarp )
		{
			AABB2 worldBounds = AABB2( m_worldCamera->GetOrthoBottomLeft(), m_worldCamera->GetOrthoTopRight() );
			if ( disc->m_center.y + disc->m_discRadius < worldBounds.m_mins.y )
			{
				float screenHeight = worldBounds.m_maxs.y - worldBounds.m_mins.y;
				disc->m_center.y = worldBounds.m_maxs.y + screenHeight * 0.1f + disc->m_discRadius;
			}
		}

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

		// Bottom
		if ( !m_isBottomWarp && disc->m_center.y - disc->m_discRadius < worldBounds.m_mins.y )
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
			TestShapeDisc* shapeDisc = dynamic_cast< TestShapeDisc* >( m_testShapes[j] );
			if ( shapeDisc != nullptr )
			{
				if ( !DoDiscsOverlap( discA->m_center, discA->m_discRadius, shapeDisc->m_center, shapeDisc->m_discRadius ) )
				{
					continue;
				}

				DiscBounceOffDisc(
					discA->m_center,
					discA->m_velocity,
					discA->m_discRadius,
					m_elasticity,
					shapeDisc->m_center,
					shapeDisc->m_discRadius,
					m_testShapes[j]->m_elasticity
				);
				continue;
			}

			TestShapeOBB2* shapeOBB2 = dynamic_cast< TestShapeOBB2* >( m_testShapes[j] );
			if ( shapeOBB2 != nullptr )
			{
				float boundingRadius = shapeOBB2->m_halfDimensions.GetLength();
				if ( !DoDiscsOverlap( discA->m_center, discA->m_discRadius, shapeOBB2->m_center, boundingRadius ) )
				{
					continue;
				}

				Vec2 closestPoint = GetNearestPointOnOBB2D( discA->m_center, *shapeOBB2->m_orientedBox );
				float smallRadius = 0.1f;
				PushBallOutOfAllBumpers( discA );
				DiscBounceOffDisc(
					discA->m_center,
					discA->m_velocity,
					discA->m_discRadius,
					m_elasticity,
					closestPoint,
					smallRadius,
					m_testShapes[j]->m_elasticity
				);
				continue;
			}

			TestShapeCapsule* capsule = dynamic_cast< TestShapeCapsule* >( m_testShapes[j] );
			if ( capsule != nullptr )
			{
				float boneLength = ( capsule->m_boneEnd - capsule->m_boneStart ).GetLength();
				float boundingRadius = boneLength * 0.5f + capsule->m_radius;
				Vec2 capsuleCenter = ( capsule->m_boneStart + capsule->m_boneEnd ) * 0.5f;
				if ( !DoDiscsOverlap( discA->m_center, discA->m_discRadius, capsuleCenter, boundingRadius ) )
				{
					continue;
				}

				Vec2 closestPoint = GetNearestPointOnCapsule2D( discA->m_center, capsule->m_boneStart, capsule->m_boneEnd, capsule->m_radius );
				float smallRadius = 0.1f;
				PushBallOutOfAllBumpers( discA );
				DiscBounceOffDisc(
					discA->m_center,
					discA->m_velocity,
					discA->m_discRadius,
					m_elasticity,
					closestPoint,
					smallRadius,
					m_testShapes[j]->m_elasticity
				);
				continue;
			}

		}
	}
}

//------------------------------------------------------------------------------
void Game2DPachinkoMachine::UpdatePhysics( float deltaSeconds )
{
	UpdateGravity( deltaSeconds );
	UpdateBounceBallsEachOther();
	UpdateBounceBallsBumpers();
	UpdateBallsWalls();
}

//------------------------------------------------------------------------------
void Game2DPachinkoMachine::UpdateGameModeKeyboardInput()
{
	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F8 ) )
	{
		ClearShapes();
		AddShapes();
	}

	if ( g_engine->m_input->WasKeyJustPressed( 'B' ) )
	{
		m_isBottomWarp = !m_isBottomWarp;
	}

	if ( g_engine->m_input->WasKeyJustPressed( 'P' ) )
	{
		m_isFixedTimeStep = !m_isFixedTimeStep;
	}

	if ( g_engine->m_input->WasKeyJustPressed( 'G' ) )
	{
		m_elasticity = GetClamped( m_elasticity - 0.05f, 0.f, 1.0f );
	}
	if ( g_engine->m_input->WasKeyJustPressed( 'H' ) )
	{
		m_elasticity = GetClamped( m_elasticity + 0.05f, 0.f, 1.0f );
	}

	if ( g_engine->m_input->WasKeyJustPressed( '[' ) )
	{
		m_fixedTimeStep /= 1.1f;
	}
	if ( g_engine->m_input->WasKeyJustPressed( ']' ) )
	{
		m_fixedTimeStep *= 1.1f;
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

//------------------------------------------------------------------------------
void Game2DPachinkoMachine::RenderInstructionText() const
{
	std::vector<Vertex> textVerts;
	float deltaSecondsMs =  (float)g_engine->m_systemClock->GetDeltaSeconds() * 1000.f;
	std::string instructionText = Stringf( "Space/N=ball(%i) | e=%0.2f(G/H) | B=warp(%s) | P=fixed(%s) | timestep=%0.2fms([/]) | T=slow | dt=%0.2fms",
		m_ballsSpawned,
		m_elasticity,
		m_isBottomWarp ? "ON" : "OFF",
		m_isFixedTimeStep ? "ON" : "OFF",
		m_fixedTimeStep * 1000.f,
		deltaSecondsMs );
	AddVertsForTextTriangles2D( textVerts, instructionText, Vec2( 1.f, 90.f ), 2.f, Rgba8( 255, 255, 255 ) );

	g_engine->m_render->BindTexture( nullptr );
	g_engine->m_render->DrawVertexArray( ( int )textVerts.size(), textVerts.data() );
}

//-----------------------------------------------------------------------------------------------
void Game2DPachinkoMachine::PushBallOutOfAllBumpers( TestShapeDisc* ball )
{
	for ( int j = 0; j < static_cast< int >( m_testShapes.size() ); ++j )
	{
		TestShapeOBB2* shapeOBB2 = dynamic_cast< TestShapeOBB2* >( m_testShapes[j] );
		if ( shapeOBB2 != nullptr )
		{
			Vec2 closestPoint = GetNearestPointOnOBB2D( ball->m_center, *shapeOBB2->m_orientedBox );
			float distToClosest = ( closestPoint - ball->m_center ).GetLength();

			if ( distToClosest < ball->m_discRadius )
			{
				Vec2 centerToClosest = closestPoint - ball->m_center;
				if ( centerToClosest.GetLengthSquared() <= 0.f )
				{
					centerToClosest = Vec2( 1.0f, 0.0f );
				}
				Vec2 normal = centerToClosest.GetNormalized();
				float overlap = ball->m_discRadius - distToClosest;
				ball->m_center -= normal * overlap;
			}
			continue;
		}

		TestShapeCapsule* capsule = dynamic_cast< TestShapeCapsule* >( m_testShapes[j] );
		if ( capsule != nullptr )
		{
			Vec2 closestPoint = GetNearestPointOnCapsule2D( ball->m_center, capsule->m_boneStart, capsule->m_boneEnd, capsule->m_radius );
			float distToClosest = ( closestPoint - ball->m_center ).GetLength();

			if ( distToClosest < ball->m_discRadius )
			{
				Vec2 centerToClosest = closestPoint - ball->m_center;
				if ( centerToClosest.GetLengthSquared() <= 0.f )
				{
					centerToClosest = Vec2( 1.0f, 0.0f );
				}
				Vec2 normal = centerToClosest.GetNormalized();
				float overlap = ball->m_discRadius - distToClosest;
				ball->m_center -= normal * overlap;
			}
			continue;
		}
	}
}