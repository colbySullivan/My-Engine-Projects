#include "Engine/Core/NamedStrings.hpp"
#include <algorithm>

//-----------------------------------------------------------------------------------------------
void NamedStrings::PopulateFromXmlElementAttributes( XmlElement const& element )
{
	XmlAttribute const* xmlAttribute = element.FirstAttribute();
	while ( xmlAttribute )
	{
		std::string key = xmlAttribute->Name();
		std::string value = xmlAttribute->Value();
		SetValue( key, value );

		xmlAttribute = xmlAttribute->Next();
	}
}

//-----------------------------------------------------------------------------------------------
void NamedStrings::SetValue( std::string const& keyName, std::string const& newValue )
{
	m_keyValuePairs[keyName] = newValue;
}

//-----------------------------------------------------------------------------------------------
std::string NamedStrings::GetValue( std::string const& keyName, std::string const& defaultValue ) const
{
	auto searchedValue = m_keyValuePairs.find( keyName );
	if ( searchedValue != m_keyValuePairs.end() )
	{
		return searchedValue->second;
	}
	return defaultValue;
}

//-----------------------------------------------------------------------------------------------
bool NamedStrings::GetValue( std::string const& keyName, bool defaultValue ) const
{
	auto searchedValue = m_keyValuePairs.find( keyName );
	if ( searchedValue != m_keyValuePairs.end() )
	{
		std::string stringBool = searchedValue->second;
		transform( stringBool.begin(), stringBool.end(), stringBool.begin(), ::tolower );
		if ( stringBool == "yes" || stringBool == "true" || stringBool == "t" )
		{
			return true;
		}
	}
	return defaultValue;
}

//-----------------------------------------------------------------------------------------------
int NamedStrings::GetValue( std::string const& keyName, int defaultValue ) const
{
	auto searchedValue = m_keyValuePairs.find( keyName );
	if ( searchedValue != m_keyValuePairs.end() )
	{
		return static_cast<int>(atoi( searchedValue->second.c_str() ));
	}
	return defaultValue;
}

//-----------------------------------------------------------------------------------------------
float NamedStrings::GetValue( std::string const& keyName, float defaultValue ) const
{
	auto searchedValue = m_keyValuePairs.find( keyName );
	if ( searchedValue != m_keyValuePairs.end() )
	{
		return static_cast<int>(atof( searchedValue->second.c_str() ));
	}
	return defaultValue;
}

//-----------------------------------------------------------------------------------------------
std::string NamedStrings::GetValue( std::string const& keyName, char const* defaultValue ) const
{
	auto searchedValue = m_keyValuePairs.find( keyName );
	if ( searchedValue != m_keyValuePairs.end() )
	{
		return searchedValue->second;
	}
	return defaultValue;
}

//-----------------------------------------------------------------------------------------------
Rgba8 NamedStrings::GetValue( std::string const& keyName, Rgba8 const& defaultValue ) const
{
	auto searchedValue = m_keyValuePairs.find( keyName );
	if ( searchedValue != m_keyValuePairs.end() )
	{
		Rgba8 resultColor;
		resultColor.SetFromText( searchedValue->second.c_str() );
		return resultColor;
	}
	return defaultValue;
}

//-----------------------------------------------------------------------------------------------
Vec2 NamedStrings::GetValue( std::string const& keyName, Vec2 const& defaultValue ) const
{
	auto searchedValue = m_keyValuePairs.find( keyName );
	if ( searchedValue != m_keyValuePairs.end() )
	{
		Vec2 resultVec2;
		resultVec2.SetFromText( searchedValue->second.c_str() );
		return resultVec2;
	}
	return defaultValue;
}

//-----------------------------------------------------------------------------------------------
IntVec2 NamedStrings::GetValue( std::string const& keyName, IntVec2 const& defaultValue ) const
{
	auto searchedValue = m_keyValuePairs.find( keyName );
	if ( searchedValue != m_keyValuePairs.end() )
	{
		IntVec2 resultIntVec2;
		resultIntVec2.SetFromText( searchedValue->second.c_str() );
		return resultIntVec2;
	}
	return defaultValue;
}