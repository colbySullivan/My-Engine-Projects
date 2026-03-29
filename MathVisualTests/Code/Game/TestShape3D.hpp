#pragma once
#include "Engine/Renderer/Texture.hpp"

struct TestShape3D
{
	TestShape3D() = default;
	virtual ~TestShape3D() = default;

	virtual void Render() const = 0;
	virtual void RenderWithTexture( Texture* texture ) const = 0;

};