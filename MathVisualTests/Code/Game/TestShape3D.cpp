#include "Game/TestShape3D.hpp"
#include "Engine/Core/Engine.hpp"

void TestShape3D::IsOverlapped() const
{
	if ( m_isOverlapping )
	{
		g_engine->m_render->SetModelConstants( Mat44(), Rgba8( 255, 0, 0 ) );
	}
	else
	{
		g_engine->m_render->SetModelConstants( Mat44(), Rgba8( 255, 255, 255 ) );
	}
}

