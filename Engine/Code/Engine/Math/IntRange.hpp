#pragma once

struct IntRange
{
public:
	int m_min = 0;
	int m_max = 0;

	static const IntRange ZERO;
	static const IntRange ONE;
	static const IntRange ZERO_TO_ONE;

	explicit IntRange(int min, int max);

	IntRange& operator=(const IntRange& other);
	bool operator==(const IntRange& other) const;
	bool operator!=(const IntRange& other) const;

	bool IsOnRange(int value) const;
	bool IsOverlappingWith(const IntRange& other) const;
};