#pragma once
#include "Engine/Renderer/Renderer.hpp"

struct OBJModel
{
	VertexBuffer* m_vbo = nullptr;
	IndexBuffer* m_ibo = nullptr;
	unsigned int  m_indexCount = 0;

	~OBJModel()
	{
		delete m_vbo;
		m_vbo = nullptr;
		delete m_ibo;
		m_ibo = nullptr;
	}
};

OBJModel* LoadOBJFromFile( const char* filePath, Renderer* renderer );