#pragma once

struct TestShape3D
{
	TestShape3D() = default;
	virtual ~TestShape3D() = default;

	virtual void Render() const = 0;
};