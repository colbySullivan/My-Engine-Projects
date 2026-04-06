#include "Game/ActorHandle.hpp"

const ActorHandle ActorHandle::INVALID;


ActorHandle::ActorHandle()
{

}

ActorHandle::ActorHandle( unsigned int uid, unsigned int index )
{
	m_data = ( uid << 16 ) | ( index & 0x0000fffeu );
}

bool ActorHandle::IsValid() const
{
	return false; // #todo implement this
}

unsigned int ActorHandle::GetIndex() const
{
	return ( m_data & 0x0000fffeu );
}


bool ActorHandle::operator!=( const ActorHandle& other ) const
{
	return false; // #todo implement this
}

bool ActorHandle::operator==( const ActorHandle& other ) const
{
	return false; // #todo implement this
}

