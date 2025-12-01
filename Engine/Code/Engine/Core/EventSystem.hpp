#pragma once
#include <string>
#include <map>
#include <vector>

typedef void (*EventSystemCallbackFunctionPtr)(); //void (*)()
typedef std::vector<EventSystemCallbackFunctionPtr> SubscriberList;
//typedef int (*AddTwoIntsPtr)( int, int );

class EventSystem
{
public:
	EventSystem() = default;
	~EventSystem() = default;
	void StartUp() {}
	void Shutdown() {}

	void BeginFrame() {}
	void EndFrame() {}

	//void SubscribeEventCallbackFunction( std::string const& eventName, void (*callbackFunction)() );
	void SubscribeEventCallbackFunction( std::string const& eventName, EventSystemCallbackFunctionPtr functionPtr );
	void FireEvent( std::string const& eventName );

private:
	//std::map< std::string, std::vector< void (*)()> > m_listOfSubscribersByEventName;
	//std::map< std::string, std::vector<EventSystemCallbackFunctionPtr()> > m_listOfSubscribersByEventName; 
	std::map< std::string, SubscriberList> m_listOfSubscribersByEventName; // These are all the same
};