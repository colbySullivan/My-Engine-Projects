#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Engine.hpp"

//------------------------------------------------------------------------------
EventSystem::EventSystem( EventSystemConfig const& config )
	: m_config( config )
{

}

//------------------------------------------------------------------------------
void EventSystem::Startup()
{

}

//------------------------------------------------------------------------------
void EventSystem::Shutdown()
{

}

//------------------------------------------------------------------------------
void EventSystem::BeginFrame()
{

}

//------------------------------------------------------------------------------
void EventSystem::EndFrame()
{

}

//-----------------------------------------------------------------------------------------------
void EventSystem::SubscribeEventCallbackFunction( std::string const& eventName, EventSystemCallbackFunctionPtr functionPtr )
{
	m_listOfSubscribersByEventName[eventName].push_back( functionPtr );
}

//------------------------------------------------------------------------------
void EventSystem::UnsubscribeEventCallbackFunction(std::string const& eventName, EventSystemCallbackFunctionPtr functionPtr)
{
	SubscriberList& subscribers = m_listOfSubscribersByEventName[eventName];

	for (int i = 0; i < static_cast<int>(subscribers.size()); i++)
	{
		if (subscribers[i] == functionPtr)
		{
			subscribers[i] = nullptr;
		}
	}
}

//-----------------------------------------------------------------------------------------------
int EventSystem::FireEvent( std::string const& eventName, EventArgs& args )
{
	SubscriberList& subscribers =  m_listOfSubscribersByEventName[ eventName ];
	for (int i = 0; i < static_cast<int>(subscribers.size()) ; ++i)
	{
		if ( subscribers[i] != nullptr ) 
		{
			bool wasConsumed = subscribers[i]( args );
			if ( wasConsumed )
			{
				return 1;
			}
		}
	}
	return 0;
}

//-----------------------------------------------------------------------------------------------
int EventSystem::FireEvent( std::string const& eventName )
{
	EventArgs args;
	return FireEvent( eventName, args );
}

//-----------------------------------------------------------------------------------------------
std::vector<std::string> EventSystem::GetAllRegisteredCommands()
{
	std::vector<std::string> registeredEventNames = { };
	for ( const auto& eventMap : m_listOfSubscribersByEventName )
	{
		registeredEventNames.push_back(eventMap.first);
	}
	return registeredEventNames;
}

//------------------------------------------------------------------------------
void SubscribeEventCallbackFunction(std::string const& eventName, EventSystemCallbackFunctionPtr functionPtr)
{
	if ( g_engine && g_engine->m_eventSystem )
	{
		g_engine->m_eventSystem->SubscribeEventCallbackFunction( eventName, functionPtr );
	}
}

//------------------------------------------------------------------------------
void UnsubscribeEventCallbackFunction(std::string const& eventName, EventSystemCallbackFunctionPtr functionPtr)
{
	if (g_engine && g_engine->m_eventSystem)
	{
		g_engine->m_eventSystem->UnsubscribeEventCallbackFunction(eventName, functionPtr);
	}
}

//-----------------------------------------------------------------------------------------------
int FireEvent( std::string const& eventName )
{
	if ( g_engine && g_engine->m_eventSystem )
	{
		EventArgs args;
		return g_engine->m_eventSystem->FireEvent( eventName, args );
	}
	return 0;
}

//------------------------------------------------------------------------------
int FireEvent(std::string const& eventName, EventArgs& args)
{
	if (g_engine && g_engine->m_eventSystem)
	{
		return g_engine->m_eventSystem->FireEvent(eventName, args);
	}
	return 0;
}