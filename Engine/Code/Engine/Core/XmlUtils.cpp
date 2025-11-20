#include "Engine/Core/XmlUtils.hpp"

//-----------------------------------------------------------------------------------------------
int XmlUtils::ParseXmlAttribute( XmlElement const& element, char const* attributeName, int defaultValue )
{
	XmlError result = element.QueryIntAttribute( attributeName, &defaultValue );
	return defaultValue;
}

//-----------------------------------------------------------------------------------------------
char XmlUtils::ParseXmlAttribute( XmlElement const& element, char const* attributeName, char defaultValue )
{
	const char* queriedChar = element.Attribute( attributeName );
	if ( *queriedChar || queriedChar[0] != '\0' ) 
	{
		return queriedChar[0];
	}
	return defaultValue;
}

//-----------------------------------------------------------------------------------------------
bool XmlUtils::ParseXmlAttribute( XmlElement const& element, char const* attributeName, bool defaultValue )
{
	const char* stringBool;
	element.QueryAttribute( attributeName, &stringBool );
	if ( stringBool == "yes" || stringBool == "true" || stringBool == "t" )
	{
		return true;
	}
	return defaultValue;
}

//-----------------------------------------------------------------------------------------------
float XmlUtils::ParseXmlAttribute( XmlElement const& element, char const* attributeName, float defaultValue )
{
	float queriedFloat = defaultValue;
	XmlError result = element.QueryFloatAttribute( attributeName, &queriedFloat );
	if ( result == 0 )
	{
		return queriedFloat;
	}
	return defaultValue;
}

//-----------------------------------------------------------------------------------------------
Rgba8 XmlUtils::ParseXmlAttribute( XmlElement const& element, char const* attributeName, Rgba8 const& defaultValue )
{
	const char* queriedRgba8;
	XmlError result = element.QueryAttribute( attributeName, &queriedRgba8 );
	if ( result == 0 )
	{
		Rgba8 queriedColor;
		queriedColor.SetFromText( queriedRgba8 );
		return queriedColor;
	}
	return defaultValue;
}

//-----------------------------------------------------------------------------------------------
Vec2 XmlUtils::ParseXmlAttribute( XmlElement const& element, char const* attributeName, Vec2 const& defaultValue )
{
	const char* queriedResult;
	XmlError result = element.QueryAttribute( attributeName, &queriedResult );
	if ( result == 0 )
	{
		Vec2 queriedPoint;
		queriedPoint.SetFromText( queriedResult );
		return queriedPoint;
	}
	return defaultValue;
}

//-----------------------------------------------------------------------------------------------
IntVec2 XmlUtils::ParseXmlAttribute( XmlElement const& element, char const* attributeName, IntVec2 const& defaultValue )
{
	const char* queriedResult;
	XmlError result = element.QueryAttribute( attributeName, &queriedResult );
	if ( result == 0 )
	{
		IntVec2 queriedIntVec;
		queriedIntVec.SetFromText( queriedResult );
		return queriedIntVec;
	}
	return defaultValue;
}

//-----------------------------------------------------------------------------------------------
std::string XmlUtils::ParseXmlAttribute( XmlElement const& element, char const* attributeName, std::string const& defaultValue )
{
	const char* queriedString;
	XmlError result = element.QueryAttribute( attributeName, &queriedString );
	if ( result == 0 )
	{
		return static_cast<std::string>( queriedString );
	}
	return defaultValue;
}

//-----------------------------------------------------------------------------------------------
std::string XmlUtils::ParseXmlAttribute( XmlElement const& element, char const* attributeName, char const* defaultValue )
{
	const char* queriedString;
	XmlError result = element.QueryAttribute( attributeName, &queriedString );
	if ( result == 0 )
	{
		return static_cast< std::string >( queriedString );
	}
	return static_cast< std::string >( defaultValue );
}

//-----------------------------------------------------------------------------------------------
Strings XmlUtils::ParseXmlAttribute( XmlElement const& element, char const* attributeName, Strings const& defaultValues, char delimiter /*= ',' */ )
{
	const char* queriedString;
	XmlError result = element.QueryAttribute( attributeName, &queriedString );
	if ( result == 0 )
	{
		return SplitStringOnDelimiter( queriedString, delimiter );
	}
	return defaultValues;
}
