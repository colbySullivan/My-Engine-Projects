#pragma once

#pragma once
#define _USE_MATH_DEFINES // Used for PI

//-----------------------------------------------------------------------------------------------
struct Vec4
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 0.f;

public:
	// Construction/Destruction
	~Vec4() {}												// destructor (do nothing)
	Vec4() {}												// default constructor (do nothing)
	Vec4( Vec4 const& copyFrom );							// copy constructor (from another Vec3)
	explicit Vec4( float initialX, float initialY, float initialZ, float initialW );		// explicit constructor (from x, y)

	// Operators (const)
	bool		operator==( Vec4 const& compare ) const;		// Vec3 == Vec3
	bool		operator!=( Vec4 const& compare ) const;		// Vec3 != Vec3
	Vec4 const	operator+( Vec4 const& vecToAdd ) const;		// Vec3 + Vec3
	Vec4 const	operator-( Vec4 const& vecToSubtract ) const;	// Vec3 - Vec3
	Vec4 const	operator-() const;								// -Vec3, i.e. "unary negation"
	Vec4 const	operator*( float uniformScale ) const;			// Vec3 * float
	Vec4 const	operator*( Vec4 const& vecToMultiply ) const;	// Vec3 * Vec3
	Vec4 const	operator/( float inverseScale ) const;			// Vec3 / float

	// Operators (self-mutating / non-const)
	void		operator+=( Vec4 const& vecToAdd );				// Vec3 += Vec3
	void		operator-=( Vec4 const& vecToSubtract );		// Vec3 -= Vec3
	void		operator*=( const float uniformScale );			// Vec3 *= float
	void		operator/=( const float uniformDivisor );		// Vec3 /= float
	void		operator=( Vec4 const& copyFrom );				// Vec3 = Vec3

	// Standalone "friend" functions that are conceptually, but not actually, part of Vec3::
	friend Vec4 const operator*( float uniformScale, Vec4 const& vecToScale );	// float * Vec3

	// Accessors (const methods)
	float		GetLength() const;
	float		GetLengthXY() const;
	float		GetLengthSquared() const;
	float		GetLengthXYSquared() const;
	float		GetOrientationAboutZDegrees() const;
	float		GetOrientationAboutZRadians() const;
	Vec4 const	GetRotatedAboutZDegrees( float rotationDegreesAboutZ ) const;
	Vec4 const	GetRotatedAboutZRadians( float rotationRadiansAboutZ ) const;
	Vec4 const	GetClampedToMaxLength( float maxLength ) const;
	Vec4 const	GetNormalized() const;

};


