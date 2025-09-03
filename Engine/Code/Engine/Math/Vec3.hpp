#pragma once


//-----------------------------------------------------------------------------------------------
struct Vec3
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;

public:
	// Construction/Destruction
	~Vec3() {}												// destructor (do nothing)
	Vec3() {}												// default constructor (do nothing)
	Vec3( Vec3 const& copyFrom );							// copy constructor (from another Vec3)
	explicit Vec3( float initialX, float initialY, float initialZ);		// explicit constructor (from x, y)

	// Operators (const)
	bool		operator==( Vec3 const& compare ) const;		// Vec3 == Vec3
	bool		operator!=( Vec3 const& compare ) const;		// Vec3 != Vec3
	Vec3 const	operator+( Vec3 const& vecToAdd ) const;		// Vec3 + Vec3
	Vec3 const	operator-( Vec3 const& vecToSubtract ) const;	// Vec3 - Vec3
	Vec3 const	operator-() const;								// -Vec3, i.e. "unary negation"
	Vec3 const	operator*( float uniformScale ) const;			// Vec3 * float
	Vec3 const	operator*( Vec3 const& vecToMultiply ) const;	// Vec3 * Vec3
	Vec3 const	operator/( float inverseScale ) const;			// Vec3 / float

	// Operators (self-mutating / non-const)
	void		operator+=( Vec3 const& vecToAdd );				// Vec3 += Vec3
	void		operator-=( Vec3 const& vecToSubtract );		// Vec3 -= Vec3
	void		operator*=( const float uniformScale );			// Vec3 *= float
	void		operator/=( const float uniformDivisor );		// Vec3 /= float
	void		operator=( Vec3 const& copyFrom );				// Vec3 = Vec3

	// Standalone "friend" functions that are conceptually, but not actually, part of Vec3::
	friend Vec3 const operator*( float uniformScale, Vec3 const& vecToScale );	// float * Vec3
};


