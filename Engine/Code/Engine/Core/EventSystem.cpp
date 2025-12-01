#include "Engine/Core/EventSystem.hpp"

//-----------------------------------------------------------------------------------------------
void EventSystem::SubscribeEventCallbackFunction( std::string const& eventName, EventSystemCallbackFunctionPtr functionPtr )
{
	m_listOfSubscribersByEventName[eventName].push_back( functionPtr );
}


//-----------------------------------------------------------------------------------------------
void EventSystem::FireEvent( std::string const& eventName )
{
	SubscriberList& subscribers =  m_listOfSubscribersByEventName[ eventName ];
	for (int i = 0; i < subscribers.size() ; ++i)
	{
		if ( subscribers[i] != nullptr ) {
			subscribers[i]();
		}
	}
}
