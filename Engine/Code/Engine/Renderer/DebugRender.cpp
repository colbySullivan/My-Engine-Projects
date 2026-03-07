#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"

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
	g_engine->m_render->BeginCamera( camera );
	g_engine->m_render->BindTexture( nullptr );

	if ( !m_debugRenderSystem->m_isVisible )
	{
		return;
	}

	for ( int objectIndex = 0; objectIndex < m_debugRenderSystem->m_worldObjects.size(); ++objectIndex )
	{
		DebugRenderObject& obj = m_debugRenderSystem->m_worldObjects[objectIndex];

		if ( !obj.m_isScreen )
		{
			if ( obj.m_isWireframe )
			{
				g_engine->m_render->SetRasterizerMode( RasterizerMode::WIREFRAME_CULL_NONE );
			}
			else
			{
				g_engine->m_render->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
			}

			if ( obj.m_mode == DebugRenderMode::ALWAYS )
			{
				g_engine->m_render->SetDepthMode( DepthMode::DISABLED );
				g_engine->m_render->DrawVertexArray( obj.m_vertices );
				break;
			}

			if ( obj.m_mode == DebugRenderMode::X_RAY )
			{
				Rgba8 ghostColor;
				ghostColor.r = ( unsigned char )( obj.m_startColor.r + ( 255 - obj.m_startColor.r ) * 0.5f );
				ghostColor.g = ( unsigned char )( obj.m_startColor.g + ( 255 - obj.m_startColor.g ) * 0.5f );
				ghostColor.b = ( unsigned char )( obj.m_startColor.b + ( 255 - obj.m_startColor.b ) * 0.5f );
				ghostColor.a = ( unsigned char )( obj.m_startColor.a * 0.25f );

				std::vector<Vertex> ghostVerts = obj.m_vertices;
				for ( int i = 0; i < ghostVerts.size(); i++ )
				{
					ghostVerts[i].m_color = ghostColor;
				}

				g_engine->m_render->SetDepthMode( DepthMode::READ_ONLY_ALWAYS );
				g_engine->m_render->SetBlendMode( BlendMode::ALPHA );
				g_engine->m_render->DrawVertexArray( ghostVerts );

				g_engine->m_render->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
				g_engine->m_render->SetBlendMode( BlendMode::OPAQUE );
				g_engine->m_render->DrawVertexArray( obj.m_vertices );
			}
			else
			{
				g_engine->m_render->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
				g_engine->m_render->SetBlendMode( BlendMode::OPAQUE );
				g_engine->m_render->DrawVertexArray( obj.m_vertices );
			}
		}

		
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

//------------------------------------------------------------------------------
void DebugAddWorldSphere( const Vec3& center, float radius, float duration, const Rgba8& startColor /*= Rgba8::WHITE*/, const Rgba8& endColor /*= Rgba8::WHITE*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	DebugRenderObject obj;
	obj.m_duration = duration;
	obj.m_startColor = startColor;
	obj.m_endColor = endColor;
	obj.m_mode = mode;
	obj.m_isScreen = false;

	Rgba8 color = startColor;
	AddVertsForSphere3D( obj.m_vertices, center, radius, 32, 32, color );

	m_debugRenderSystem->m_worldObjects.push_back( obj );
}

//------------------------------------------------------------------------------
void DebugAddWorldWireSphere( const Vec3& center, float radius, float duration, const Rgba8& startColor /*= Rgba8::WHITE*/, const Rgba8& endColor /*= Rgba8::WHITE*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	DebugRenderObject obj;
	obj.m_duration = duration;
	obj.m_startColor = startColor;
	obj.m_endColor = endColor;
	obj.m_mode = mode;
	obj.m_isScreen = false;
	obj.m_isWireframe = true;


	Rgba8 color = startColor;
	AddVertsForSphere3D( obj.m_vertices, center, radius, 32, 32, color );

	m_debugRenderSystem->m_worldObjects.push_back( obj );
}

//------------------------------------------------------------------------------
void DebugAddWorldCylinder( const Vec3& start, const Vec3& end, float radius, float duration, const Rgba8& startColor /*= Rgba8::WHITE*/, const Rgba8& endColor /*= Rgba8::WHITE*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	DebugRenderObject obj;
	obj.m_duration = duration;
	obj.m_startColor = startColor;
	obj.m_endColor = endColor;
	obj.m_mode = mode;
	obj.m_isScreen = false;

	AddVertsForCylinder3D( obj.m_vertices, start, end, radius, startColor );

	m_debugRenderSystem->m_worldObjects.push_back( obj );
}

//------------------------------------------------------------------------------
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
	if ( m_debugRenderSystem->m_isVisible )
	{
		DebugRenderSetHidden();
	}
	else
	{
		DebugRenderSetVisible();
	}
	return false;
}
