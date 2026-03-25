#include "Game/Actor.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"

//-----------------------------------------------------------------------------------------------
Actor::Actor( Game* owner, Vec3 start, Vec3 end, float radius, int numSlices )
{
	m_game = owner;
	AddVertsForCylinder3D( m_vertexes, start, end, radius, m_color, AABB2::ZERO_TO_ONE, numSlices );
	//AddVertsForCylinder3D( m_vertexes, Vec3( -10.f, 5.f, 0.f ), Vec3( 10.f, 5.f, 2.f ), 0.5f, m_color, AABB2::ZERO_TO_ONE, 32 );
}

//-----------------------------------------------------------------------------------------------
Actor::~Actor()
{

}

//-----------------------------------------------------------------------------------------------
void Actor::Update( float deltaSeconds )
{
	m_orientation.m_pitchDegrees += m_angularVelocity.m_pitchDegrees * deltaSeconds;
	m_orientation.m_rollDegrees += m_angularVelocity.m_rollDegrees * deltaSeconds;
	m_orientation.m_yawDegrees += m_angularVelocity.m_yawDegrees * deltaSeconds;
}

//-----------------------------------------------------------------------------------------------
void Actor::Render() const
{
	g_engine->m_render->SetModelConstants( GetModelToWorldTransform(), m_color );
	g_engine->m_render->m_desiredBlendMode = BlendMode::OPAQUE;
	g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::WIREFRAME_CULL_BACK;
	g_engine->m_render->BindTexture( m_texture );
	g_engine->m_render->DrawVertexArray( ( int )m_vertexes.size(), m_vertexes.data() );

	// #TODO need to make this so there are two objects one that is wireframe and one that is full // Maybe use set model constant and draw wire white first and then set to red
	//Rgba8 newColor = Rgba8( 122,122,122,255 );
	//for (int vertexIndex = 0; vertexIndex < m_vertexes.size() ; ++vertexIndex)
	//{
	//	
	//	m_vertexes[vertexIndex].m_color.r = newColor.r;
	//}
	//g_engine->m_render->m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	//g_engine->m_render->DrawVertexArray( ( int )m_vertexes.size(), m_vertexes.data() );
}

//------------------------------------------------------------------------------
Mat44 Actor::GetModelToWorldTransform() const
{
	Mat44 model = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
	model.SetTranslation3D( m_position );
	return model;
}