#include "Game/TestShapes3D.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "TestShapeSphere.hpp"
#include "TestShapeAABB3.hpp"
#include "TestShapeCylinder.hpp"

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
	m_player->Update( ( float )g_engine->m_systemClock->GetDeltaSeconds() );
	UpdateSpawnNewTestShapes();
	UpdateShapesOverlap();
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

	g_engine->m_render->EndCamera( *m_player->m_worldCamera );
	g_engine->m_render->SetBlendMode( BlendMode::OPAQUE );
	g_engine->m_render->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
	g_engine->m_render->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
}

//------------------------------------------------------------------------------
void TestShapes3D::RenderTestShapes() const
{
	for ( int shapeIndex = 0; shapeIndex < static_cast< int >( m_testShapes.size() ); ++shapeIndex )
	{
		TestShape3D* shape = m_testShapes[shapeIndex];
		if ( shape != nullptr )
		{
			shape->RenderWithTexture( m_modelTexture );
		}
	}
}

//------------------------------------------------------------------------------
void TestShapes3D::SpawnInitialTestShapes()
{
	DebugAddWorldBasis( m_player->GetModelToWorldTransform(), 0.f );
	float halfHeight = 0.5f;
	Mat44 toWorld = m_player->GetModelToWorldTransform();
	Vec3 upVector = toWorld.GetKBasis3D();
	Vec3 startPos = m_player->m_position - ( upVector * halfHeight );
	Vec3 endPos = m_player->m_position + ( upVector * halfHeight );

	TestShapeSphere* sphereShape = new TestShapeSphere( Vec3( 10.f, 10.f, 0.f ), 1.0f, 32, 32 );
	m_testShapes.push_back( sphereShape );
	m_testShapeSpheres.push_back( sphereShape );

	TestShapeAABB3* aabb3Shape = new TestShapeAABB3( AABB3( Vec3( -10.f, -10.f, -10.f ), Vec3( 10.f, 10.f, 10.f ) ) );
	m_testShapes.push_back( aabb3Shape );
	m_testShapeAABB3.push_back( aabb3Shape );

	TestShapeCylinder* cylinderShape = new TestShapeCylinder( startPos, endPos, 0.25f );
	m_testShapes.push_back( cylinderShape );
	m_testShapeCylinder.push_back( cylinderShape );
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
