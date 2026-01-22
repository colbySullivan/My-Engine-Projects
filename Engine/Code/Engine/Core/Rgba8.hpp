#pragma once

struct Rgba8
{
public:
	unsigned char r = 255; // Default values to opaque white
	unsigned char g = 255;
	unsigned char b = 255;
	unsigned char a = 255;

public:
	Rgba8() {}																												// default constructor (do nothing)
	explicit Rgba8(unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte=255);		// explicit constructor (from x, y)
	void SetFromText( char const* text );

	static const Rgba8 WHITE;
	void GetAsFloats( float* colorAsFloats ) const;
};

Rgba8 Interpolate( Rgba8 start, Rgba8 end, float fractionOfEnd );