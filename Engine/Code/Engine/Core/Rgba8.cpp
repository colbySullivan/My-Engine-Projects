#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"

Rgba8 const WHITE(255, 255, 255, 255);

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
	r = static_cast<unsigned char>(atof( splitVec2[0].c_str() ));
	g = static_cast<unsigned char>(atof( splitVec2[1].c_str() ));
	b = static_cast<unsigned char>(atof( splitVec2[2].c_str() ));
	if ( splitVec2.size() == 4 )
	{
		a = static_cast<unsigned char>(atof( splitVec2[3].c_str() ));
	}
}

//-----------------------------------------------------------------------------------------------
void Rgba8::GetAsFloats( float* colorAsFloats ) const
{
	colorAsFloats[0] = RangeMap( ( float )r, 0.f, 255.f, 0.f, 1.f );
	colorAsFloats[1] = RangeMap( ( float )g, 0.f, 255.f, 0.f, 1.f );
	colorAsFloats[2] = RangeMap( ( float )b, 0.f, 255.f, 0.f, 1.f );
	colorAsFloats[3] = RangeMap( ( float )a, 0.f, 255.f, 0.f, 1.f );
}

//-----------------------------------------------------------------------------------------------
Rgba8 Interpolate( Rgba8 start, Rgba8 end, float fractionOfEnd )
{
	float r = Interpolate( NormalizeByte( start.r ), NormalizeByte( end.r ), fractionOfEnd );
	float g = Interpolate( NormalizeByte( start.g ), NormalizeByte( end.g ), fractionOfEnd );
	float b = Interpolate( NormalizeByte( start.b ), NormalizeByte( end.b ), fractionOfEnd );
	float a = Interpolate( NormalizeByte( start.a ), NormalizeByte( end.a ), fractionOfEnd );
	return Rgba8( DenormalizeByte(r), DenormalizeByte(g), DenormalizeByte(b) ,DenormalizeByte(a) );
}