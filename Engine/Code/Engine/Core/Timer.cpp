#include "Engine/Core/Timer.hpp"

//-----------------------------------------------------------------------------------------------
Timer::Timer( double period, const Clock* clock /*= nullptr */ )
	: m_period( period )
	, m_clock( clock )
{
	if ( clock == nullptr )
	{
		m_clock = &Clock::GetSystemClock();
	}
}

//-----------------------------------------------------------------------------------------------
void Timer::Start()
{
	if ( m_clock )
	{
		m_startTime = m_clock->GetTotalSeconds();
	}
}

//-----------------------------------------------------------------------------------------------
void Timer::Stop()
{
	m_startTime = -1.0;
}

//-----------------------------------------------------------------------------------------------
double Timer::GetElapsedTime() const
{
	if ( IsStopped() )
	{
		return 0.0;
	}
	return m_clock->GetTotalSeconds() - m_startTime;
}

//-----------------------------------------------------------------------------------------------
double Timer::GetElapsedFraction() const
{
	return GetElapsedTime() / m_period;
}

//-----------------------------------------------------------------------------------------------
bool Timer::IsStopped() const
{
	return m_startTime < 0.0;
}

//-----------------------------------------------------------------------------------------------
bool Timer::HasPeriodElapsed() const
{
	if ( IsStopped() )
	{
		return false;
	}
	return GetElapsedTime() >= m_period;
}

//-----------------------------------------------------------------------------------------------
bool Timer::DecrementPeriodIfElapsed()
{
	if ( !HasPeriodElapsed() )
	{
		return false;
	}

	m_startTime += m_period;
	return true;
}

