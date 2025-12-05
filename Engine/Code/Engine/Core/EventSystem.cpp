#include "Engine/Core/EventSystem.hpp"

//-----------------------------------------------------------------------------------------------
void EventSystem::SubscribeEventCallbackFunction( std::string const& eventName, EventSystemCallbackFunctionPtr functionPtr )
{
	m_listOfSubscribersByEventName[eventName].push_back( functionPtr );
}


//-----------------------------------------------------------------------------------------------
void EventSystem::FireEvent( std::string const& eventName, EventArgs& args )
{
	SubscriberList& subscribers =  m_listOfSubscribersByEventName[ eventName ];
	for (int i = 0; i < subscribers.size() ; ++i)
	{
		if ( subscribers[i] != nullptr ) 
		{
			bool wasConsumed = subscribers[i]( args );
			if ( wasConsumed )
			{
				break; // don't call any other callbacks; event was consumed
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
void EventSystem::FireEvent( std::string const& eventName )
{
	EventArgs args;
	FireEvent( eventName, args );
}

//-----------------------------------------------------------------------------------------------
int FireEvent( std::string const& eventName )
{
	//return Engine->event system FireEvent
	return 0;
}