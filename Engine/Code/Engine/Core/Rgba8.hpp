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
	static const Rgba8 BLACK;
	static const Rgba8 RED;
	static const Rgba8 GREEN;
	static const Rgba8 BLUE;
	static const Rgba8 YELLOW;

	void GetAsFloats( float* colorAsFloats ) const;

	bool operator==(const Rgba8& other) const;
};

Rgba8 Interpolate( Rgba8 start, Rgba8 end, float fractionOfEnd );