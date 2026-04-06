#include "Game/TestShape3D.hpp"
#include "Engine/Core/Engine.hpp"


void TestShape3D::IsOverlapped() const
{
	if ( m_isOverlapping )
	{
		float flash = 0.5f + 0.5f * SinDegrees( (float) m_shapeBlinkTimer->GetElapsedFraction() * 360.f );
		unsigned char alpha = ( unsigned char )( flash * 127.f + 127.f );
		g_engine->m_render->SetModelConstants( Mat44(), Rgba8( alpha, alpha, alpha, alpha ) );
	}
	else
	{
		g_engine->m_render->SetModelConstants( Mat44(), Rgba8( 255, 255, 255, 255 ) );
	}

	if ( m_shapeBlinkTimer->DecrementPeriodIfElapsed() )
	{
		m_shapeBlinkTimer->Start();
	}
}

//------------------------------------------------------------------------------
void TestShape3D::IsCasted() const
{
	if ( m_isClosestRaycast )
	{
		g_engine->m_render->SetModelConstants( Mat44(), Rgba8( 255, 0, 0 ) );
	}
}

