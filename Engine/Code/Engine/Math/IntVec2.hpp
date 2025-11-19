#pragma once

//------------------------------------------------------------------------------

struct IntVec2
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid
	int x = 0;
	int y = 0;

public:
	// Construction/Destruction
	~IntVec2() {}                                        // destructor (do nothing)
	IntVec2() {}                                         // default constructor (do nothing)
	IntVec2(IntVec2 const& copyFrom);                  // copy constructor (from another IntVec2)
	explicit IntVec2(int initialX, int initialY);      // explicit constructor (from x, y)

	// Accessors (const methods)
	float           GetLength() const;
	int             GetTaxicabLength() const;
	int             GetLengthSquared() const;
	float           GetOrientationRadians() const;
	float           GetOrientationDegrees() const;
	IntVec2 const   GetRotatedBy90Degrees() const;
	IntVec2 const   GetRotatedByMinus90Degrees() const;

	// Mutators (non-const methods)
	void            Rotate90Degrees();
	void            RotateMinus90Degrees();
	void			SetFromText( char const* text );

	// Operators (self-mutating / non-const)
	void operator=(const IntVec2& copyFrom);           // IntVec2 = IntVec2
	IntVec2 const operator+(IntVec2 const& vecToAdd) const;  // IntVec2 + IntVec2
};