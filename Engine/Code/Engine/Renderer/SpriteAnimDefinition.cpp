#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

//-----------------------------------------------------------------------------------------------
SpriteAnimDefinition::SpriteAnimDefinition( SpriteSheet const& sheet, int startSpriteIndex, int endSpriteIndex, float framesPerSecond, SpriteAnimPlaybackType playbackType /*= SpriteAnimPlaybackType::LOOP */ )
	: m_spriteSheet ( sheet )
	, m_startSpriteIndex ( startSpriteIndex )
	, m_endSpriteIndex ( endSpriteIndex )
	, m_framesPerSecond ( framesPerSecond )
	, m_playbackType ( playbackType )
{

}

//-----------------------------------------------------------------------------------------------
SpriteDefinition const& SpriteAnimDefinition::GetSpriteDefAtTime( float seconds ) const
{
	int frameIndex = -1;
	int numOfFrames = m_endSpriteIndex - m_startSpriteIndex + 1;
	if ( m_playbackType == SpriteAnimPlaybackType::LOOP )
	{
		frameIndex = static_cast< int >( seconds / m_framesPerSecond ) % numOfFrames + m_startSpriteIndex;
	}

	else if ( m_playbackType == SpriteAnimPlaybackType::ONCE )
	{
		frameIndex = static_cast< int >( seconds / m_framesPerSecond ) + m_startSpriteIndex;
		if ( frameIndex > m_endSpriteIndex )
		{
			frameIndex = m_endSpriteIndex;
		}
	}

	else if ( m_playbackType == SpriteAnimPlaybackType::PINGPONG )
	{
		int pingPongNumOfFrames = (numOfFrames - 1) * 2;
		int currentFrame = static_cast< int >( seconds / m_framesPerSecond ) % pingPongNumOfFrames;

		if ( currentFrame < numOfFrames )
		{
			frameIndex = currentFrame + m_startSpriteIndex;
		}
		else
		{
			frameIndex = (pingPongNumOfFrames - currentFrame) + m_startSpriteIndex;
		}
	}

	return m_spriteSheet.GetSpriteDef( frameIndex );
}