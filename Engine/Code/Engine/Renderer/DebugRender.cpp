#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/VertexUtils.hpp"

struct DebugRenderObject
{
	std::vector<Vertex>		m_vertices;
	float					m_duration = 0.f;
	float					m_age = 0.f;
	Rgba8					m_startColor;
	Rgba8					m_endColor;
	DebugRenderMode			m_mode = DebugRenderMode::USE_DEPTH;
	bool					m_isScreen = false;
	bool					m_isWireframe = false;
};

struct DebugRenderSystem
{
	DebugRenderConfig				m_config;
	bool							m_isVisible = true;
	std::vector<DebugRenderObject>	m_worldObjects;
	std::vector<DebugRenderObject>	m_screenObjects;
};

static DebugRenderSystem* m_debugRenderSystem = nullptr;

//-----------------------------------------------------------------------------------------------
void DebugRenderSystemStartup( const DebugRenderConfig& config )
{
	m_debugRenderSystem = new DebugRenderSystem();
	m_debugRenderSystem->m_config = config;
}

//-----------------------------------------------------------------------------------------------
void DebugRenderSystemShutdown()
{
	delete m_debugRenderSystem;
	m_debugRenderSystem = nullptr;
}

//-----------------------------------------------------------------------------------------------
void DebugRenderSetVisible()
{
	m_debugRenderSystem->m_isVisible = true;

}

//-----------------------------------------------------------------------------------------------
void DebugRenderSetHidden()
{
	m_debugRenderSystem->m_isVisible = false;
}

//-----------------------------------------------------------------------------------------------
void DebugRenderClear()
{
	m_debugRenderSystem->m_worldObjects.clear();
	m_debugRenderSystem->m_screenObjects.clear();
}

//-----------------------------------------------------------------------------------------------
void DebugRenderBeginFrame()
{
	
}

//-----------------------------------------------------------------------------------------------
void DebugRenderWorld( const Camera& camera )
{
	Renderer* r = m_debugRenderSystem->m_config.m_renderer;
	r->BeginCamera( camera );
	r->BindTexture( nullptr );

	if ( !m_debugRenderSystem->m_isVisible )
	{
		return;
	}

	for (int objectIndex = 0; objectIndex < m_debugRenderSystem->m_worldObjects.size() ; ++objectIndex)
	{
		DebugRenderObject& obj = m_debugRenderSystem->m_worldObjects[objectIndex];

		if ( obj.m_mode == DebugRenderMode::ALWAYS )
		{
			r->SetDepthMode( DepthMode::DISABLED );
		}

		if ( obj.m_mode == DebugRenderMode::X_RAY )
		{
			r->SetDepthMode( DepthMode::READ_ONLY_LESS_EQUAL );
			r->SetBlendMode( BlendMode::ALPHA );
			r->DrawVertexArray( obj.m_vertices );
			Rgba8 startColor = obj.m_startColor;
			Rgba8 ghostColor = obj.m_startColor;
			ghostColor.a = ( unsigned char )( ghostColor.a * 0.25f );
			obj.m_startColor.a = ghostColor.a;
			r->DrawVertexArray( obj.m_vertices );

			r->SetBlendMode( BlendMode::OPAQUE );
			r->SetDepthMode( DepthMode::READ_ONLY_ALWAYS );
			obj.m_startColor = startColor;
		}

		r->SetRasterizerMode( RasterizerMode::WIREFRAME_CULL_NONE );
		r->DrawVertexArray( obj.m_vertices );
	}
}

//-----------------------------------------------------------------------------------------------
void DebugRenderScreen( const Camera& camera )
{

}

//-----------------------------------------------------------------------------------------------
void DebugRenderEndFrame()
{

}

void DebugAddWorldWireCylinder( const Vec3& start, const Vec3& end, float radius, float duration, const Rgba8& startColor /*= Rgba8::WHITE*/, const Rgba8& endColor /*= Rgba8::WHITE*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	DebugRenderObject obj;
	obj.m_duration = duration;
	obj.m_startColor = startColor;
	obj.m_endColor = endColor;
	obj.m_mode = mode;
	obj.m_isScreen = false;
	obj.m_isWireframe = true;

	AddVertsForCylinder3D( obj.m_vertices, start, end, radius, startColor );

	m_debugRenderSystem->m_worldObjects.push_back( obj );
}

//-----------------------------------------------------------------------------------------------
bool Command_DebugRenderToggle( EventArgs& args )
{
	DebugRenderSetHidden();
	return false;
}
