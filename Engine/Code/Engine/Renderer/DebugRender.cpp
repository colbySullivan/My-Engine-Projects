#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"

struct DebugRenderObject
{
	std::vector<Vertex>		m_vertices;
	float					m_duration = 0.f;
	float					m_age = 0.f;
	Rgba8					m_startColor;
	Rgba8					m_endColor;
	DebugRenderMode			m_mode = DebugRenderMode::USE_DEPTH;
	bool					m_isScreenMessage = false;
	bool					m_isWireframe = false;
	Texture*				m_texture = nullptr;
	Timer*					m_timer = nullptr;
	BillboardType			m_billboardType = BillboardType::NONE;
	Vec3 					m_position = Vec3( 0.f ,0.f, 0.f );
	int						m_messageIndex = -1;
	std::string				m_text = "";
};

struct DebugRenderSystem
{
	DebugRenderConfig				m_config;
	bool							m_isVisible = true;
	std::vector<DebugRenderObject>	m_worldObjects;
	std::vector<DebugRenderObject>	m_screenObjects;
	int								m_systemMessageIndex = 0;
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

	if ( !m_debugRenderSystem->m_isVisible )
	{
		g_engine->m_render->EndCamera( camera );
		return;
	}

	for ( int objectIndex = 0; objectIndex < m_debugRenderSystem->m_worldObjects.size(); ++objectIndex )
	{
		DebugRenderObject& obj = m_debugRenderSystem->m_worldObjects[objectIndex];
		g_engine->m_render->BindTexture( obj.m_texture );

		if ( obj.m_billboardType != BillboardType::NONE )
		{
			std::vector<Vertex> originalVerts = obj.m_vertices;
			Mat44 billboardMat = GetBillboardTransform( obj.m_billboardType, camera.GetCameraToWorldTransform(), obj.m_position );
			TransformVertexArray3D( originalVerts, billboardMat );
			if ( obj.m_isWireframe && (obj.m_billboardType == BillboardType::WORLD_UP_OPPOSING || obj.m_billboardType == BillboardType::FULL_OPPOSING) )
			{
				g_engine->m_render->SetRasterizerMode( RasterizerMode::WIREFRAME_CULL_NONE );
			}
			if ( obj.m_billboardType == BillboardType::WORLD_UP_OPPOSING || obj.m_billboardType == BillboardType::FULL_OPPOSING )
			{
				g_engine->m_render->SetRasterizerMode( RasterizerMode::SOLID_CULL_NONE );
			}
			g_engine->m_render->DrawVertexArray( originalVerts );
			continue;
		}

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
		}
		else if ( obj.m_mode == DebugRenderMode::X_RAY )
		{
			Rgba8 ghostColor;
			ghostColor.r = ( unsigned char )( obj.m_startColor.r + ( 255 - obj.m_startColor.r ) * 0.5f );
			ghostColor.g = ( unsigned char )( obj.m_startColor.g + ( 255 - obj.m_startColor.g ) * 0.5f );
			ghostColor.b = ( unsigned char )( obj.m_startColor.b + ( 255 - obj.m_startColor.b ) * 0.5f );
			ghostColor.a = ( unsigned char )( obj.m_startColor.a * 0.25f );
			std::vector<Vertex> ghostVerts = obj.m_vertices;
			for ( int i = 0; i < ( int )ghostVerts.size(); i++ )
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

	g_engine->m_render->EndCamera( camera );
}

//-----------------------------------------------------------------------------------------------
void DebugRenderScreen( const Camera& camera )
{
	g_engine->m_render->BeginCamera( camera );

	AABB2 cameraBounds = AABB2(camera.GetOrthoBottomLeft(), camera.GetOrthographicTopRight());
	float screenWidth = cameraBounds.m_maxs.x - cameraBounds.m_mins.x;
	float cellHeight = 10.f;

	std::string fontPath = m_debugRenderSystem->m_config.m_fontPath + m_debugRenderSystem->m_config.m_fontName;
	BitmapFont* font = g_engine->m_render->CreateOrGetBitmapFont( fontPath.c_str() );

	for ( int lineIndex = 0; lineIndex < m_debugRenderSystem->m_screenObjects.size(); ++lineIndex )
	{
		DebugRenderObject& obj = m_debugRenderSystem->m_screenObjects[lineIndex];
		g_engine->m_render->BindTexture( obj.m_texture );

		if ( obj.m_isScreenMessage && font )
		{
			int visualRow = 0;
			for ( int otherIndex = 0; otherIndex < m_debugRenderSystem->m_screenObjects.size(); ++otherIndex )
			{
				const DebugRenderObject& other = m_debugRenderSystem->m_screenObjects[otherIndex];
				if ( other.m_isScreenMessage && other.m_messageIndex < obj.m_messageIndex )
				{
					visualRow++;
				}
			}

			float yTop = cameraBounds.m_maxs.y - ( visualRow * cellHeight ) - 20.f;
			float yBottom = yTop - cellHeight;
			AABB2 box( Vec2( cameraBounds.m_mins.x, yBottom ), Vec2( cameraBounds.m_mins.x + screenWidth * 0.5f, yTop ) );

			std::vector<Vertex> verts;
			Rgba8 currentColor = obj.m_startColor;
			if ( obj.m_duration > 0.f )
			{
				float ageFraction = obj.m_age / obj.m_duration;
				currentColor.r = ( unsigned char )( obj.m_startColor.r + ( obj.m_endColor.r - obj.m_startColor.r ) * ageFraction );
				currentColor.g = ( unsigned char )( obj.m_startColor.g + ( obj.m_endColor.g - obj.m_startColor.g ) * ageFraction );
				currentColor.b = ( unsigned char )( obj.m_startColor.b + ( obj.m_endColor.b - obj.m_startColor.b ) * ageFraction );
				currentColor.a = ( unsigned char )( obj.m_startColor.a + ( obj.m_endColor.a - obj.m_startColor.a ) * ageFraction );
			}
			font->AddVertsForTextInBox2D( verts, obj.m_text, box, cellHeight, currentColor, 1.0f, Vec2( 0.f, 0.5f ), TextBoxMode::SHRINK_TO_FIT );
			g_engine->m_render->DrawVertexArray( verts );
		}
		else
		{
			g_engine->m_render->DrawVertexArray( obj.m_vertices );
		}
	}

	g_engine->m_render->EndCamera( camera );
}

//-----------------------------------------------------------------------------------------------
void DebugRenderEndFrame()
{
	for ( int objectIndex = 0; objectIndex < m_debugRenderSystem->m_worldObjects.size(); ++objectIndex )
	{
		DebugRenderObject& obj = m_debugRenderSystem->m_worldObjects[objectIndex];
		if ( obj.m_timer != nullptr )
		{
			if ( obj.m_timer->DecrementPeriodIfElapsed() )
			{
				delete obj.m_timer;
				obj.m_timer = nullptr;
				m_debugRenderSystem->m_worldObjects.erase( m_debugRenderSystem->m_worldObjects.begin() + objectIndex );
				objectIndex--;
			}
			else
			{
				obj.m_age += ( float )g_engine->m_systemClock->GetDeltaSeconds();
				float ageFraction = obj.m_age / obj.m_duration;
				unsigned char r = ( unsigned char )( obj.m_startColor.r + ( obj.m_endColor.r - obj.m_startColor.r ) * ageFraction );
				unsigned char g = ( unsigned char )( obj.m_startColor.g + ( obj.m_endColor.g - obj.m_startColor.g ) * ageFraction );
				unsigned char b = ( unsigned char )( obj.m_startColor.b + ( obj.m_endColor.b - obj.m_startColor.b ) * ageFraction );
				unsigned char a = ( unsigned char )( obj.m_startColor.a + ( obj.m_endColor.a - obj.m_startColor.a ) * ageFraction );
				for ( int vertIndex = 0; vertIndex < obj.m_vertices.size(); vertIndex++ )
				{
					obj.m_vertices[vertIndex].m_color = Rgba8( r, g, b, a );
				}
			}
		}
	}

	for ( int objectIndex = 0; objectIndex < m_debugRenderSystem->m_screenObjects.size(); ++objectIndex )
	{
		DebugRenderObject& obj = m_debugRenderSystem->m_screenObjects[objectIndex];
		if ( obj.m_timer != nullptr )
		{
			if ( obj.m_timer->DecrementPeriodIfElapsed() )
			{
				delete obj.m_timer;
				obj.m_timer = nullptr;
				m_debugRenderSystem->m_screenObjects.erase( m_debugRenderSystem->m_screenObjects.begin() + objectIndex );
				objectIndex--;
			}
			else
			{
				obj.m_age += ( float )g_engine->m_systemClock->GetDeltaSeconds();
				float ageFraction = obj.m_age / obj.m_duration;
				unsigned char r = ( unsigned char )( obj.m_startColor.r + ( obj.m_endColor.r - obj.m_startColor.r ) * ageFraction );
				unsigned char g = ( unsigned char )( obj.m_startColor.g + ( obj.m_endColor.g - obj.m_startColor.g ) * ageFraction );
				unsigned char b = ( unsigned char )( obj.m_startColor.b + ( obj.m_endColor.b - obj.m_startColor.b ) * ageFraction );
				unsigned char a = ( unsigned char )( obj.m_startColor.a + ( obj.m_endColor.a - obj.m_startColor.a ) * ageFraction );
				for ( int vertIndex = 0; vertIndex < obj.m_vertices.size(); vertIndex++ )
				{
					obj.m_vertices[vertIndex].m_color = Rgba8( r, g, b, a );
				}
			}
		}
	}
}

//------------------------------------------------------------------------------
void DebugAddWorldSphere( const Vec3& center, float radius, float duration, const Rgba8& startColor /*= Rgba8::WHITE*/, const Rgba8& endColor /*= Rgba8::WHITE*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	DebugRenderObject obj;
	obj.m_duration = duration;
	obj.m_startColor = startColor;
	obj.m_endColor = endColor;
	obj.m_mode = mode;

	if ( duration > 0.f )
	{
		obj.m_timer = new Timer( duration );
		obj.m_timer->Start();
	}
	if ( duration == 0.f )
	{
		obj.m_timer = new Timer( g_engine->m_systemClock->GetDeltaSeconds() ); 
		obj.m_timer->Start();
	}

	Rgba8 color = startColor;
	AddVertsForSphere3D( obj.m_vertices, center, radius, 16, 16, color );

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
	obj.m_isWireframe = true;

	if ( duration > 0.f )
	{
		obj.m_timer = new Timer( duration );
		obj.m_timer->Start();
	}
	if ( duration == 0.f )
	{
		obj.m_timer = new Timer( 0.0001f );
		obj.m_timer->Start();
	}

	Rgba8 color = startColor;
	AddVertsForSphere3D( obj.m_vertices, center, radius, 16, 16, color );

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

	if ( duration > 0.f )
	{
		obj.m_timer = new Timer( duration );
		obj.m_timer->Start();
	}
	if ( duration == 0.f )
	{
		obj.m_timer = new Timer( g_engine->m_systemClock->GetDeltaSeconds() );
		obj.m_timer->Start();
	}

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
	obj.m_isWireframe = true;

	if ( duration > 0.f )
	{
		obj.m_timer = new Timer( duration );
		obj.m_timer->Start();
	}
	if ( duration == 0.f )
	{
		obj.m_timer = new Timer( g_engine->m_systemClock->GetDeltaSeconds() );
		obj.m_timer->Start();
	}

	AddVertsForCylinder3D( obj.m_vertices, start, end, radius, startColor );

	m_debugRenderSystem->m_worldObjects.push_back( obj );
}

//------------------------------------------------------------------------------
void DebugAddWorldArrow( const Vec3& start, const Vec3& end, float radius, float duration, const Rgba8& startColor /*= Rgba8::WHITE*/, const Rgba8& endColor /*= Rgba8::WHITE*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	DebugRenderObject obj;
	obj.m_duration = duration;
	obj.m_startColor = startColor;
	obj.m_endColor = endColor;
	obj.m_mode = mode;

	if ( duration > 0.f )
	{
		obj.m_timer = new Timer( duration );
		obj.m_timer->Start();
	}
	if ( duration == 0.f )
	{
		obj.m_timer = new Timer( g_engine->m_systemClock->GetDeltaSeconds() );
		obj.m_timer->Start();
	}

	AddVertsForArrow3D( obj.m_vertices, start, end, radius, startColor );
	m_debugRenderSystem->m_worldObjects.push_back( obj );
}

//------------------------------------------------------------------------------
void DebugAddWorldWireArrow( const Vec3& start, const Vec3& end, float radius, float duration, const Rgba8& startColor /*= Rgba8::WHITE*/, const Rgba8& endColor /*= Rgba8::WHITE*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	DebugRenderObject obj;
	obj.m_duration = duration;
	obj.m_startColor = startColor;
	obj.m_endColor = endColor;
	obj.m_mode = mode;
	obj.m_isWireframe = true;

	if ( duration > 0.f )
	{
		obj.m_timer = new Timer( duration );
		obj.m_timer->Start();
	}
	if ( duration == 0.f )
	{
		obj.m_timer = new Timer( g_engine->m_systemClock->GetDeltaSeconds() );
		obj.m_timer->Start();
	}

	AddVertsForArrow3D( obj.m_vertices, start, end, radius, startColor );

	m_debugRenderSystem->m_worldObjects.push_back( obj );
}

void DebugAddBasis( const Mat44& transform, float duration, float length, float radius, float colorScale /*= 1.0f*/, float alphaScale /*= 1.0f*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	DebugRenderObject obj;
	obj.m_duration = duration;
	obj.m_mode = mode;

	if ( duration > 0.f )
	{
		obj.m_timer = new Timer( duration );
		obj.m_timer->Start();
	}
	if ( duration == 0.f )
	{
		obj.m_timer = new Timer( g_engine->m_systemClock->GetDeltaSeconds() );
		obj.m_timer->Start();
	}

	float arrowLength = 1.f;
	Vec3 origin = transform.GetTranslation3D();
	AddVertsForArrow3D( obj.m_vertices, origin, origin + transform.GetIBasis3D().GetNormalized() * arrowLength, .1f, Rgba8( 255, 0, 0 ) );
	AddVertsForArrow3D( obj.m_vertices, origin, origin + transform.GetJBasis3D().GetNormalized() * arrowLength, .1f, Rgba8( 0, 255, 0 ) );
	AddVertsForArrow3D( obj.m_vertices, origin, origin + transform.GetKBasis3D().GetNormalized() * arrowLength, .1f, Rgba8( 0, 0, 255 ) );

	Mat44 xLabelTransform = transform;
	xLabelTransform.SetTranslation3D( transform.GetTranslation3D() + transform.GetIBasis3D().GetNormalized() * arrowLength ); // #TODO need to move offset
	DebugAddWorldText( "X", xLabelTransform, 0.2f, Vec2( 0.5f, 0.5f ), duration, Rgba8( 255, 0, 0 ), Rgba8( 255, 0, 0 ), mode );

	Mat44 yLabelTransform = transform;
	yLabelTransform.SetTranslation3D( transform.GetTranslation3D() + transform.GetJBasis3D().GetNormalized() * arrowLength );
	DebugAddWorldText( "Y", yLabelTransform, 0.2f, Vec2( 0.5f, 0.5f ), duration, Rgba8( 0, 255, 0 ), Rgba8( 0, 255, 0 ), mode );

	Mat44 zLabelTransform = transform;
	zLabelTransform.SetTranslation3D( transform.GetTranslation3D() + transform.GetKBasis3D().GetNormalized() * arrowLength );
	DebugAddWorldText( "Z", zLabelTransform, 0.2f, Vec2( 0.5f, 0.5f ), duration, Rgba8( 0, 0, 255 ), Rgba8( 0, 0, 255 ), mode );

	m_debugRenderSystem->m_worldObjects.push_back( obj );
}

//-----------------------------------------------------------------------------------------------
void DebugAddWorldBasis( const Mat44& transform, float duration, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	DebugRenderObject obj;
	obj.m_duration = duration;
	obj.m_mode = mode;

	if ( duration > 0.f )
	{
		obj.m_timer = new Timer( duration );
		obj.m_timer->Start();
	}
	if ( duration == 0.f )
	{
		obj.m_timer = new Timer( g_engine->m_systemClock->GetDeltaSeconds() );
		obj.m_timer->Start();
	}

	float arrowLength = 1.f;
	AddVertsForArrow3D( obj.m_vertices, Vec3::ZERO, transform.GetIBasis3D().GetNormalized() * arrowLength, .1f, Rgba8( 255, 0, 0 ) );
	AddVertsForArrow3D( obj.m_vertices, Vec3::ZERO, transform.GetJBasis3D().GetNormalized() * arrowLength, .1f, Rgba8( 0, 255, 0 ) );
	AddVertsForArrow3D( obj.m_vertices, Vec3::ZERO, transform.GetKBasis3D().GetNormalized() * arrowLength, .1f, Rgba8( 0, 0, 255 ) );

	Mat44 xLabelTransform = transform;
	xLabelTransform.SetTranslation3D( transform.GetTranslation3D() + transform.GetIBasis3D().GetNormalized() * arrowLength ); // #TODO need to move offset
	DebugAddWorldText( "X", xLabelTransform, 0.2f, Vec2( 0.5f, 0.5f ), duration, Rgba8( 255, 0, 0 ), Rgba8( 255, 0, 0 ), mode );

	Mat44 yLabelTransform = transform;
	yLabelTransform.SetTranslation3D( transform.GetTranslation3D() + transform.GetJBasis3D().GetNormalized() * arrowLength );
	DebugAddWorldText( "Y", yLabelTransform, 0.2f, Vec2( 0.5f, 0.5f ), duration, Rgba8( 0, 255, 0 ), Rgba8( 0, 255, 0 ), mode );

	Mat44 zLabelTransform = transform;
	zLabelTransform.SetTranslation3D( transform.GetTranslation3D() + transform.GetKBasis3D().GetNormalized() * arrowLength );
	DebugAddWorldText( "Z", zLabelTransform, 0.2f, Vec2( 0.5f, 0.5f ), duration, Rgba8( 0, 0, 255 ), Rgba8( 0, 0, 255 ), mode );

	m_debugRenderSystem->m_worldObjects.push_back( obj );
}

void DebugAddWorldText( const std::string& text, const Mat44& transform, float textHeight, const Vec2& alignment, float duration, const Rgba8& startColor /*= Rgba8::WHITE*/, const Rgba8& endColor /*= Rgba8::WHITE*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	std::string fontPath = m_debugRenderSystem->m_config.m_fontPath + m_debugRenderSystem->m_config.m_fontName;
	BitmapFont* font = g_engine->m_render->CreateOrGetBitmapFont( fontPath.c_str() );
	if ( !font )
	{
		ERROR_AND_DIE( Stringf( "Failed to load font for DebugRender: %s", fontPath.c_str() ) );
	}

	std::vector<Vertex> verts;
	font->AddVertsForText3DAtOriginXForward( verts, textHeight, text, startColor, 1.f, alignment );
	TransformVertexArray3D( verts, transform );

	DebugRenderObject obj;
	obj.m_vertices = verts;
	obj.m_duration = duration;
	obj.m_startColor = startColor;
	obj.m_endColor = endColor;
	obj.m_texture = &font->GetTexture();
	obj.m_billboardType = BillboardType::NONE;
	obj.m_mode = mode;

	if ( duration > 0.f )
	{
		obj.m_timer = new Timer( duration );
		obj.m_timer->Start();
	}
	if ( duration == 0.f )
	{
		obj.m_timer = new Timer( g_engine->m_systemClock->GetDeltaSeconds() );
		obj.m_timer->Start();
	}

	m_debugRenderSystem->m_worldObjects.push_back( obj );
}

void DebugAddWorldBillboardText( const std::string& text, const Vec3& origin, float textHeight, const Vec2& alignment, float duration, const Rgba8& startColor /*= Rgba8::WHITE*/, const Rgba8& endColor /*= Rgba8::WHITE*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	std::string fontPath = m_debugRenderSystem->m_config.m_fontPath + m_debugRenderSystem->m_config.m_fontName;
	BitmapFont* font = g_engine->m_render->CreateOrGetBitmapFont( fontPath.c_str() );
	if ( !font )
	{
		ERROR_AND_DIE( Stringf( "Failed to load font for DebugRender: %s", fontPath.c_str() ) );
	}
	std::vector<Vertex> verts;
	font->AddVertsForText3DAtOriginXForward( verts, textHeight, text, startColor, 1.f, alignment );

	DebugRenderObject obj;
	obj.m_vertices = verts;
	obj.m_duration = duration;
	obj.m_startColor = startColor;
	obj.m_endColor = endColor;
	obj.m_texture = &font->GetTexture();
	obj.m_billboardType = BillboardType::FULL_OPPOSING;
	obj.m_position = origin;

	if ( duration > 0.f )
	{
		obj.m_timer = new Timer( duration );
		obj.m_timer->Start();
	}
	if ( duration == 0.f )
	{
		obj.m_timer = new Timer( g_engine->m_systemClock->GetDeltaSeconds() );
		obj.m_timer->Start();
	}

	m_debugRenderSystem->m_worldObjects.push_back( obj );
}

//------------------------------------------------------------------------------
void DebugAddScreenText( const std::string& text, const AABB2& box, float cellHeight, const Vec2& alignment, float duration, const Rgba8& startColor /*= Rgba8::WHITE*/, const Rgba8& endColor /*= Rgba8::WHITE */ )
{
	std::string fontPath = m_debugRenderSystem->m_config.m_fontPath + m_debugRenderSystem->m_config.m_fontName;

    BitmapFont* font = g_engine->m_render->CreateOrGetBitmapFont( fontPath.c_str() );
    if ( !font )
    {
        ERROR_AND_DIE( Stringf( "Failed to load font for DebugRender: %s", fontPath.c_str() ) );
    }

    std::vector<Vertex> verts;
    font->AddVertsForTextInBox2D( verts, text, box, cellHeight, startColor, 1.0f, alignment, TextBoxMode::SHRINK_TO_FIT );

    DebugRenderObject obj;
    obj.m_vertices = verts;
    obj.m_duration = duration;
    obj.m_startColor = startColor;
    obj.m_endColor = endColor;
    obj.m_isScreenMessage = false;
    obj.m_isWireframe = false;
	obj.m_texture = &font->GetTexture();

	if ( duration > 0.f )
	{
		obj.m_timer = new Timer( duration );
		obj.m_timer->Start();
	}
	if ( duration == 0.f )
	{
		obj.m_timer = new Timer( g_engine->m_systemClock->GetDeltaSeconds() );
		obj.m_timer->Start();
	}

    m_debugRenderSystem->m_screenObjects.push_back( obj );
}

//------------------------------------------------------------------------------
void DebugAddMessage( const std::string& text, float duration, const Rgba8& startColor /*= Rgba8::WHITE*/, const Rgba8& endColor /*= Rgba8::WHITE */ )
{
	std::string fontPath = m_debugRenderSystem->m_config.m_fontPath + m_debugRenderSystem->m_config.m_fontName;
	BitmapFont* font = g_engine->m_render->CreateOrGetBitmapFont( fontPath.c_str() );
	if ( !font )
	{
		ERROR_AND_DIE( Stringf( "Failed to load font for DebugRender: %s", fontPath.c_str() ) );
	}

	DebugRenderObject obj;
	obj.m_duration = duration;
	obj.m_startColor = startColor;
	obj.m_endColor = endColor;
	obj.m_isScreenMessage = true;
	obj.m_texture = &font->GetTexture();
	obj.m_messageIndex = m_debugRenderSystem->m_systemMessageIndex++;

	if ( duration > 0.f )
	{
		obj.m_timer = new Timer( duration );
		obj.m_timer->Start();
	}
	if ( duration == 0.f )
	{
		obj.m_timer = new Timer( g_engine->m_systemClock->GetDeltaSeconds() );
		obj.m_timer->Start();
	}

	obj.m_text = text;

	m_debugRenderSystem->m_screenObjects.push_back( obj );
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
