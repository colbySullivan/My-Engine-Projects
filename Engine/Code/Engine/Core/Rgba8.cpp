#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"

//-----------------------------------------------------------------------------------------------
Rgba8::Rgba8(unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte)
	: r(redByte)
	, g(greenByte)
	, b(blueByte)
	, a(alphaByte)
{
}

//-----------------------------------------------------------------------------------------------
void Rgba8::SetFromText( char const* text )
{
	Strings splitVec2 = SplitStringOnDelimiter( text, ',' );
	r = atof( splitVec2[0].c_str() );
	g = atof( splitVec2[1].c_str() );
	b = atof( splitVec2[2].c_str() );
	if ( splitVec2.size() == 4 )
	{
		a = atof( splitVec2[3].c_str() );
	}
}