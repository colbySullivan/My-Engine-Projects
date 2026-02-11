#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Time.hpp"

static Clock* g_systemClock = new Clock();

//-----------------------------------------------------------------------------------------------
Clock::Clock()
	: m_parent( g_systemClock )
{
}

//-----------------------------------------------------------------------------------------------
Clock::Clock( Clock& parent )
	: m_parent( &parent )
{
	m_parent->AddChild( this );
}

//-----------------------------------------------------------------------------------------------
Clock::~Clock()
{
	if ( m_parent )
	{
		m_parent->RemoveChild( this );
	}

	for (int childIndex = 0; childIndex < m_children.size() ; ++childIndex)
	{
		Clock* child = m_children[childIndex];
		if ( child )
		{
			child->m_parent = nullptr;
		}
	}
	m_children.clear();
	g_systemClock = nullptr;
}

//-----------------------------------------------------------------------------------------------
void Clock::Reset()
{
	m_lastUpdateTimeInSeconds = 0.0;
	m_totalSeconds = 0.0f;
	m_deltaSeconds = 0.0;
	m_frameCount = 0;

	if ( m_parent == nullptr )
	{
		m_lastUpdateTimeInSeconds = GetCurrentTimeSeconds();
	}
	else
	{
		m_lastUpdateTimeInSeconds = m_parent->GetTotalSeconds();
	}
}

//-----------------------------------------------------------------------------------------------
bool Clock::IsPaused() const
{
	return m_isPaused;
}

//-----------------------------------------------------------------------------------------------
void Clock::Pause()
{
	m_isPaused = true;
}

//-----------------------------------------------------------------------------------------------
void Clock::Unpause()
{
	m_isPaused = false;
}

//-----------------------------------------------------------------------------------------------
void Clock::TogglePause()
{
	m_isPaused = !m_isPaused;
}

//-----------------------------------------------------------------------------------------------
void Clock::StepSingleFrame()
{
	m_stepSingleFrame = true;
	m_isPaused = false;
}

//-----------------------------------------------------------------------------------------------
void Clock::SetTimeScale( double timeScale )
{
	m_timeScale = timeScale;
}

//-----------------------------------------------------------------------------------------------
double Clock::GetTimeScale() const
{
	return m_timeScale;
}

//-----------------------------------------------------------------------------------------------
double Clock::GetDeltaSeconds() const
{
	return m_deltaSeconds;
}

//-----------------------------------------------------------------------------------------------
double Clock::GetTotalSeconds() const
{
	return m_totalSeconds;
}

//-----------------------------------------------------------------------------------------------
double Clock::GetFrameRate() const
{
	if ( m_deltaSeconds <= 0.0 )
	{
		return 0.0;
	}
	return 1.0 / m_deltaSeconds;
}

//-----------------------------------------------------------------------------------------------
int Clock::GetFrameCount() const
{
	return m_frameCount;
}

//-----------------------------------------------------------------------------------------------
Clock& Clock::GetSystemClock()
{
	return *g_systemClock;
}

//-----------------------------------------------------------------------------------------------
void Clock::TickSystemClock()
{
	g_systemClock->Tick();
}

//-----------------------------------------------------------------------------------------------
void Clock::Tick()
{
	double currentTimeInSeconds = 0.0;
	if ( m_parent == nullptr )
	{
		currentTimeInSeconds = GetCurrentTimeSeconds();
	}
	else
	{
		currentTimeInSeconds = m_parent->GetTotalSeconds();
	}

	double deltaSeconds = currentTimeInSeconds - m_lastUpdateTimeInSeconds;
	if ( deltaSeconds > m_maxDeltaSeconds )
	{
		deltaSeconds = m_maxDeltaSeconds;
	}

	m_lastUpdateTimeInSeconds = currentTimeInSeconds;
	Advance( deltaSeconds );
}

//-----------------------------------------------------------------------------------------------
void Clock::Advance( double deltaTimeSeconds )
{
	if ( m_isPaused )
	{
		if ( m_stepSingleFrame )
		{
			m_stepSingleFrame = false;
		}
		else
		{
			m_deltaSeconds = 0.0;

			for ( int childIndex = 0; childIndex < m_children.size(); ++childIndex )
			{
				Clock* child = m_children[childIndex];
				if ( child )
				{
					child->Advance( m_deltaSeconds );
				}
			}
			return;
		}
	}

	m_deltaSeconds = deltaTimeSeconds * m_timeScale;
	m_totalSeconds += m_deltaSeconds;
	m_frameCount++;

	for ( int childIndex = 0; childIndex < m_children.size(); ++childIndex )
	{
		Clock* child = m_children[childIndex];
		if ( child )
		{
			child->Advance( m_deltaSeconds );
		}
	}

	if ( m_stepSingleFrame )
	{
		m_isPaused = true;
		m_stepSingleFrame = false;
	}
}

//-----------------------------------------------------------------------------------------------
void Clock::AddChild( Clock* childClock )
{
	if ( childClock == nullptr )
	{
		return;
	}

	for (int childIndex = 0; childIndex < m_children.size() ; ++childIndex)
	{
		if ( childClock == m_children[childIndex] )
		{
			return;
		}
	}
	m_children.push_back( childClock );
}

//-----------------------------------------------------------------------------------------------
void Clock::RemoveChild( Clock* childClock )
{
	if ( childClock == nullptr )
	{
		return;
	}
	delete childClock;
}