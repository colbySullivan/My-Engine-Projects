#include "Game/TestShapes3D.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "TestShapeSphere.hpp"
#include "TestShapeAABB3.hpp"
#include "TestShapeCylinder.hpp"

RandomNumberGenerator rng;

//-----------------------------------------------------------------------------------------------
TestShapes3D::TestShapes3D( App* app )
	: Game(app)
{
	m_player = new Player( this );
	m_player->m_position = Vec3( 0.f, 0.f, 1.f );
	m_raycastStartPos = m_player->m_position;
	m_modelTexture = g_engine->m_render->CreateOrGetTextureFromFile( "Data/Textures/Test_StbiFlippedAndOpenGL.png" );
	m_screenCamera = new Camera;
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( 1600.f, 800.f ) );

}

//-----------------------------------------------------------------------------------------------
TestShapes3D::~TestShapes3D()
{
	Shutdown();
	delete m_player;
}

//------------------------------------------------------------------------------
void TestShapes3D::Startup()
{
	Game::Startup();
	SpawnInitialTestShapes();
}

//------------------------------------------------------------------------------
void TestShapes3D::Shutdown()
{
	ClearTestShapes();
}

//------------------------------------------------------------------------------
void TestShapes3D::Update( float deltaSeconds )
{
	m_player->Update( deltaSeconds );
	UpdateShapes();
	UpdateSpawnNewTestShapes();
	UpdateShapesOverlap();
	UpdateClosePoints();
	RaycastTestShapes();
	UpdateMoveRaycast();
	UpdateMoveClosetShape();
	Game::UpdateKeyboardInput();
}

//------------------------------------------------------------------------------
void TestShapes3D::Render() const
{
	g_engine->m_render->BindTexture( nullptr );
	g_engine->m_render->BeginCamera( *m_player->m_worldCamera );
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	Rgba8 backgroundColor = Rgba8( static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ), static_cast< unsigned char >( 0.f ) ); // Suppresses error with conversion
	g_engine->m_render->ClearScreen( backgroundColor );
	DebugRenderWorld( *m_player->m_worldCamera );

	RenderTestShapes();
	RenderBasis();

	g_engine->m_render->EndCamera( *m_player->m_worldCamera );
	RenderUI();
	g_engine->m_render->SetBlendMode( BlendMode::OPAQUE );
	g_engine->m_render->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
	g_engine->m_render->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
}

//------------------------------------------------------------------------------
void TestShapes3D::RenderTestShapes() const
{
	for ( int shapeIndex = 0; shapeIndex < static_cast< int >( m_testShapesTextured.size() ); ++shapeIndex )
	{
		TestShape3D* shape = m_testShapesTextured[shapeIndex];
		if ( shape != nullptr )
		{
			shape->RenderWithTexture( m_modelTexture );
		}
	}

	for ( int shapeIndex = 0; shapeIndex < static_cast< int >( m_testShapesWired.size() ); ++shapeIndex )
	{
		TestShape3D* shape = m_testShapesWired[shapeIndex];
		if ( shape != nullptr )
		{
			shape->Render();
		}
	}
}

//-----------------------------------------------------------------------------------------------
void TestShapes3D::RenderUI() const
{
	float screenSizeY = 800.f;
	float screenSizeX = 1600.f;
	std::string hudText = Stringf( "Mode (F6/F7 for prev/next): Test Shapes 3D" );
	DebugAddScreenText( hudText, AABB2( Vec2( 0.f, screenSizeY - 25.f ), Vec2( screenSizeX, screenSizeY ) ), 15.f, Vec2( 0.f, 0.5f ), 0.f, Rgba8( 255, 255, 0 ), Rgba8( 255, 255, 0 ) );
	
	std::string hudText2 = Stringf( "F8 to Randomize; ESDF = fly horizontal, AZ = fly vertical, space = lock raycast, hold t = slow") ;
	if ( m_closestShape != nullptr )
	{
		hudText2 = Stringf( "F8 to Randomize; ESDF = fly horizontal, AZ = fly vertical, space = lock raycast, hold t = slow, LMB = grab object" );
	}
	DebugAddScreenText( hudText2, AABB2( Vec2( 0.f, screenSizeY - 55.f ), Vec2( screenSizeX, screenSizeY ) ), 15.f, Vec2( 0.f, 0.5f ), 0.f, Rgba8( 0, 255, 255 ), Rgba8( 0, 255, 255 ) );

	DebugRenderScreen( *m_screenCamera );
}

//------------------------------------------------------------------------------
void TestShapes3D::SpawnInitialTestShapes()
{
	DebugAddWorldBasis( m_player->GetModelToWorldTransform(), 0.f );

	int numSpheres = 0;
	int numAABBs = 6;
	int numCylinders = 0;

	for ( int i = 0; i < numSpheres; ++i )
	{
		float radius = rng.RollRandomFloatInRange( 0.3f, 2.0f );
		bool  isWired = ( i % 2 == 0 );

		TestShapeSphere* shape = new TestShapeSphere( RandomPoint(), radius, 16, 16 );
		m_testShapes.push_back( shape );
		m_testShapeSpheres.push_back( shape );
		if ( isWired )
			m_testShapesWired.push_back( shape );
		else
			m_testShapesTextured.push_back( shape );
	}

	for ( int i = 0; i < numAABBs; ++i )
	{
		Vec3  center = RandomPoint();
		float halfW = rng.RollRandomFloatInRange( 0.3f, 1.5f );
		float halfD = rng.RollRandomFloatInRange( 0.3f, 1.5f );
		float halfH = rng.RollRandomFloatInRange( 0.3f, 1.5f );
		bool  isWired = ( i % 2 == 0 );

		AABB3 box( center - Vec3( halfW, halfD, halfH ),
			center + Vec3( halfW, halfD, halfH ) );

		TestShapeAABB3* shape = new TestShapeAABB3( box );
		m_testShapes.push_back( shape );
		m_testShapeAABB3.push_back( shape );
		if ( isWired )
			m_testShapesWired.push_back( shape );
		else
			m_testShapesTextured.push_back( shape );
	}

	for ( int i = 0; i < numCylinders; ++i )
	{
		Vec3  base = RandomPoint();
		float height = rng.RollRandomFloatInRange( 0.5f, 3.0f );
		float radius = rng.RollRandomFloatInRange( 0.1f, 0.6f );
		bool  isWired = ( i % 2 == 0 );

		Vec3 start = base;
		Vec3 end = base + Vec3( 0.f, 0.f, height );

		TestShapeCylinder* shape = new TestShapeCylinder( start, end, radius );
		m_testShapes.push_back( shape );
		m_testShapeCylinder.push_back( shape );
		if ( isWired )
			m_testShapesWired.push_back( shape );
		else
			m_testShapesTextured.push_back( shape );
	}
}

void TestShapes3D::UpdateSpawnNewTestShapes()
{
	if ( g_engine->m_input->WasKeyJustPressed( '1' ) )
	{
		Mat44 toWorld = Mat44();
		Vec3 upVector = toWorld.GetKBasis3D();
		float halfHeight = 0.5f;
		Vec3 startPos = m_player->m_position - ( upVector * halfHeight );
		Vec3 endPos = m_player->m_position + ( upVector * halfHeight );
		TestShapeCylinder* cylinderShape = new TestShapeCylinder( startPos, endPos, 0.25f );
		m_testShapes.push_back( cylinderShape );
		m_testShapeCylinder.push_back( cylinderShape );
	}

	if ( g_engine->m_input->WasKeyJustPressed( '2' ) )
	{
		TestShapeSphere* sphereShape = new TestShapeSphere( m_player->m_position, 1.0f, 32, 32 );
		m_testShapes.push_back( sphereShape );
		m_testShapeSpheres.push_back( sphereShape );
	}

	if ( g_engine->m_input->WasKeyJustPressed( '3' ) )
	{
		Vec3 boundsMins = m_player->m_position - Vec3( 0.5f, 0.5f, 0.5f );
		Vec3 boundsMaxs = m_player->m_position + Vec3( 0.5f, 0.5f, 0.5f );
		TestShapeAABB3* aabb3Shape = new TestShapeAABB3( AABB3( boundsMins, boundsMaxs ) );
		m_testShapes.push_back( aabb3Shape );
		m_testShapeAABB3.push_back( aabb3Shape );
	}

}

void TestShapes3D::ClearTestShapes()
{
	for ( int shapeIndex = 0; shapeIndex < static_cast< int >( m_testShapes.size() ); ++shapeIndex )
	{
		TestShape3D* shape = m_testShapes[shapeIndex];
		if ( shape != nullptr )
		{
			delete shape;
			m_testShapes[shapeIndex] = nullptr;
		}
	}
}

void TestShapes3D::UpdateShapesOverlap()
{
	for ( int shapeIndex = 0; shapeIndex < static_cast< int >( m_testShapes.size() ); ++shapeIndex )
	{
		TestShape3D* shape = m_testShapes[shapeIndex];
		if ( UpdateShapesOverlapWithSphere( shape ) )			shape->m_isOverlapping = true;
		else if ( UpdateShapesOverlapWithCylinder( shape ) )	shape->m_isOverlapping = true;
		else if ( UpdateShapesOverlapWithAABB3( shape ) )		shape->m_isOverlapping = true;
		else													shape->m_isOverlapping = false;
	}
}

//------------------------------------------------------------------------------
bool TestShapes3D::UpdateShapesOverlapWithSphere( TestShape3D* shape )
{
	for ( int sphereShapeIndex = 0; sphereShapeIndex < static_cast< int >( m_testShapeSpheres.size() ); ++sphereShapeIndex )
	{
		TestShapeSphere* otherShape = m_testShapeSpheres[sphereShapeIndex];
		if ( shape != nullptr && otherShape != nullptr && shape != otherShape )
		{
			if ( shape->DoesSphereOverlap( otherShape->m_center, otherShape->m_radius ) )
			{
				return true;
			}
		}
	}
	return false;
}

//------------------------------------------------------------------------------
bool TestShapes3D::UpdateShapesOverlapWithCylinder( TestShape3D* shape )
{
	for ( int cylinderShapeIndex = 0; cylinderShapeIndex < static_cast< int >( m_testShapeCylinder.size() ); ++cylinderShapeIndex )
	{
		TestShapeCylinder* otherShape = m_testShapeCylinder[cylinderShapeIndex];
		if ( shape != nullptr && otherShape != nullptr && shape != otherShape )
		{
			if ( shape->DoesCylinderOverlap( Vec2( otherShape->m_center.x, otherShape->m_center.y ), otherShape->m_radius, otherShape->m_zRange ) )
			{
				return true;
			}
		}
	}
	return false;
}

//------------------------------------------------------------------------------
bool TestShapes3D::UpdateShapesOverlapWithAABB3( TestShape3D* shape )
{
	for ( int AABB3ShapeIndex = 0; AABB3ShapeIndex < static_cast< int >( m_testShapeAABB3.size() ); ++AABB3ShapeIndex )
	{
		TestShapeAABB3* otherShape = m_testShapeAABB3[AABB3ShapeIndex];
		if ( shape != nullptr && otherShape != nullptr && shape != otherShape )
		{
			if ( shape->DoesAABBOverlap( Vec3( otherShape->m_bounds.m_mins.x, otherShape->m_bounds.m_mins.y, otherShape->m_bounds.m_mins.z ), Vec3( otherShape->m_bounds.m_maxs.x, otherShape->m_bounds.m_maxs.y, otherShape->m_bounds.m_maxs.z ) ) )
			{
				return true;
			}
		}
	}
	return false;
}

//------------------------------------------------------------------------------
void TestShapes3D::RaycastTestShapes()
{
	if ( !m_isLeftClickMoveMode ) 
	{
		m_closestShape = nullptr;
		float closestImpactDist = 999999.f;
		m_shortestResult = RaycastResult3D();
		for ( int shapeIndex = 0; shapeIndex < static_cast< int >( m_testShapes.size() ); ++shapeIndex )
		{
			TestShape3D* shape = m_testShapes[shapeIndex];
			shape->m_isClosestRaycast = false;
			if ( shape != nullptr )
			{
				RaycastResult3D result = shape->RaycastTestShape( m_raycastStartPos, m_savedForwardRaycastNormal, 10.f );
				if ( result.m_didImpact )
				{
					if ( result.m_impactDist < closestImpactDist )
					{
						closestImpactDist = result.m_impactDist;
						m_closestShape = shape;
						m_shortestResult = result;
					}
				}
			}
		}
		if ( m_closestShape != nullptr )
		{
			m_closestShape->m_isClosestRaycast = true;

		}
		if ( m_shortestResult.m_didImpact )
		{
			float deltaSeconds = ( float )g_engine->m_systemClock->GetDeltaSeconds();
			DebugAddWorldSphere( m_shortestResult.m_impactPos, 0.05f, deltaSeconds, Rgba8( 255, 255, 255 ), Rgba8( 255, 255, 255 ) );
			Vec3 raycastEndPos = m_shortestResult.m_impactPos + ( m_shortestResult.m_impactNormal * 1.0f );
			DebugAddWorldArrow( m_shortestResult.m_impactPos, raycastEndPos, 0.01f, deltaSeconds, Rgba8( 0, 255, 255 ), Rgba8( 0, 255, 255 ) );
			if ( !m_isRaycastMoveMode )
			{
				DebugAddWorldCylinder( m_shortestResult.m_impactPos, m_raycastStartPos, 0.01f, deltaSeconds, Rgba8( 0, 100, 255 ), Rgba8( 0, 100, 255 ) );
			}
		}
	}
}

//------------------------------------------------------------------------------
void TestShapes3D::RenderBasis() const
{
	Mat44 toWorld = m_player->GetModelToWorldTransform();
	Vec3 playerPos = toWorld.GetTranslation3D();
	Vec3 playerForward = toWorld.GetIBasis3D().GetNormalized();

	float offsetDistance = 0.5f;
	Vec3 indicatorPos = playerPos + playerForward * offsetDistance;

	Mat44 worldBasisTransform;
	worldBasisTransform.SetTranslation3D( indicatorPos );

	DebugAddBasis( worldBasisTransform, 0.f, 0.02f, 0.001f );
}

//------------------------------------------------------------------------------
void TestShapes3D::UpdateMoveClosetShape()
{
	if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_LEFT_MOUSE ) )
	{
		m_isRaycastMoveMode = true;
		if ( !m_isLeftClickMoveMode && m_closestShape != nullptr )
		{
			m_grabbedShape = m_closestShape;
			Mat44 toWorld = m_player->GetModelToWorldTransform();
			Vec3 forwardNormal = toWorld.GetIBasis3D().GetNormalized();
			m_grabbedDistance = ( m_grabbedShape->GetCenter() - m_player->m_position ).GetLength();
		}
		else
		{
			m_grabbedShape = nullptr;
		}
		m_isLeftClickMoveMode = !m_isLeftClickMoveMode;
	}

	if ( !m_isLeftClickMoveMode || m_grabbedShape == nullptr )
		return;

	Mat44 toWorld = m_player->GetModelToWorldTransform();
	Vec3 forwardNormal = toWorld.GetIBasis3D().GetNormalized();
	Vec3 newPos = m_player->m_position + ( forwardNormal * m_grabbedDistance );

	if ( TestShapeSphere* sphereShape = dynamic_cast< TestShapeSphere* >( m_grabbedShape ) )
	{
		sphereShape->m_center = newPos;
	}
	else if ( TestShapeCylinder* cylinderShape = dynamic_cast< TestShapeCylinder* >( m_grabbedShape ) )
	{
		float halfHeight = ( cylinderShape->m_end - cylinderShape->m_start ).GetLength() * 0.5f;
		cylinderShape->m_center = newPos;
		cylinderShape->m_start = newPos - ( Vec3::Z_AXIS * halfHeight );
		cylinderShape->m_end = newPos + ( Vec3::Z_AXIS * halfHeight );
	}
	else if ( TestShapeAABB3* aabb3Shape = dynamic_cast< TestShapeAABB3* >( m_grabbedShape ) )
	{
		Vec3 half = ( aabb3Shape->m_bounds.m_maxs - aabb3Shape->m_bounds.m_mins ) * 0.5f;
		aabb3Shape->m_bounds.m_mins = newPos - half;
		aabb3Shape->m_bounds.m_maxs = newPos + half;
	}
}

//------------------------------------------------------------------------------
void TestShapes3D::UpdateShapes()
{
	for ( int shapeIndex = 0; shapeIndex < static_cast< int >( m_testShapes.size() ); ++shapeIndex )
	{
		TestShape3D* shape = m_testShapes[shapeIndex];
		if ( shape != nullptr )
		{
			shape->Update();
		}
	}
}

Vec3 TestShapes3D::RandomPoint()
{
	float spawnRadius = 20.f;
	float spawnHeight = 10.f;

	float x = rng.RollRandomFloatInRange( -spawnRadius, spawnRadius );
	float y = rng.RollRandomFloatInRange( -spawnRadius, spawnRadius );
	float z = rng.RollRandomFloatInRange( 0.f, spawnHeight );
	return Vec3( x, y, z );
}

//------------------------------------------------------------------------------
void TestShapes3D::UpdateClosePoints()
{
	for ( int shapeIndex = 0; shapeIndex < static_cast< int >( m_testShapes.size() ); ++shapeIndex )
	{
		TestShape3D* shape = m_testShapes[shapeIndex];
		if ( shape != nullptr )
		{
			float deltaSeconds = ( float )g_engine->m_systemClock->GetDeltaSeconds();
			Vec3 playerPosition = m_player->m_position;
			Vec3 closestPoint = shape->GetClosestPoint( playerPosition );
			DebugAddWorldSphere( closestPoint, 0.05f, deltaSeconds, Rgba8( 255, 255, 0 ), Rgba8( 255, 255, 0 ) );
		}
	}
}

//------------------------------------------------------------------------------
void TestShapes3D::UpdateMoveRaycast()
{
	if ( g_engine->m_input->WasKeyJustPressed( ' ' ) )
	{
		m_isRaycastMoveMode = !m_isRaycastMoveMode;
	}

	if ( m_isRaycastMoveMode ) 
	{
		Mat44 rotationMatrix = m_player->m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
		m_savedForwardRaycastNormal = rotationMatrix.GetIBasis3D().GetNormalized();
		m_raycastStartPos = m_player->m_position;
	}
}
