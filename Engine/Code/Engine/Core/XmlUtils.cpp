#include "Engine/Core/XmlUtils.hpp"

//-----------------------------------------------------------------------------------------------
XmlUtils::XmlUtils()
{
	
}

//-----------------------------------------------------------------------------------------------
XmlUtils::~XmlUtils()
{
}

//-----------------------------------------------------------------------------------------------
int XmlUtils::ParseXmlAttribute( XmlElement const& element, char const* attributeName, int defaultValue )
{
	int queriedInt = defaultValue;
	XmlError result = element.QueryIntAttribute( attributeName, &queriedInt );
	if ( result )
	{
		return queriedInt;
	}
	return defaultValue;
}

//-----------------------------------------------------------------------------------------------
char XmlUtils::ParseXmlAttribute( XmlElement const& element, char const* attributeName, char defaultValue )
{
	const char* queriedChar = element.Attribute( attributeName );
	if ( *queriedChar || queriedChar[0] != '\0' ) {
		return queriedChar[0];
	}
	return defaultValue;
}

//-----------------------------------------------------------------------------------------------
bool XmlUtils::ParseXmlAttribute( XmlElement const& element, char const* attributeName, bool defaultValue )
{
	return element.BoolAttribute( attributeName, defaultValue );
}

//-----------------------------------------------------------------------------------------------
float XmlUtils::ParseXmlAttribute( XmlElement const& element, char const* attributeName, float defaultValue )
{
	float queriedFloat = defaultValue;
	XmlError result = element.QueryFloatAttribute( attributeName, &queriedFloat );
	if ( result )
	{
		return queriedFloat;
	}
	return defaultValue;
}

//-----------------------------------------------------------------------------------------------
Rgba8 XmlUtils::ParseXmlAttribute( XmlElement const& element, char const* attributeName, Rgba8 const& defaultValue )
{
	const char* queriedFloat;
	XmlError result = element.QueryAttribute( attributeName, &queriedFloat );
	if ( result )
	{
		//Rgba8 foundRgba8 = Rgba8( queriedFloat[]); // Need ti splitstring
	}
	return defaultValue;
}

//-----------------------------------------------------------------------------------------------
Vec2 XmlUtils::ParseXmlAttribute( XmlElement const& element, char const* attributeName, Vec2 const& defaultValue )
{
	return defaultValue;
}

//-----------------------------------------------------------------------------------------------
IntVec2 XmlUtils::ParseXmlAttribute( XmlElement const& element, char const* attributeName, IntVec2 const& defaultValue )
{
	return defaultValue;
}

//-----------------------------------------------------------------------------------------------
std::string XmlUtils::ParseXmlAttribute( XmlElement const& element, char const* attributeName, std::string const& defaultValue )
{
	return defaultValue;
}

//-----------------------------------------------------------------------------------------------
std::string XmlUtils::ParseXmlAttribute( XmlElement const& element, char const* attributeName, char const* defaultValue )
{
	return defaultValue;
}

//-----------------------------------------------------------------------------------------------
Strings XmlUtils::ParseXmlAttribute( XmlElement const& element, char const* attributeName, Strings const& defaultValues, char delimiter /*= ',' */ )
{
	return defaultValues;
}
