#include "Engine/Renderer/Model.hpp"
#include "Engine/Core/Engine.hpp"

//------------------------------------------------------------------------------
Model::~Model()
{
	delete m_vertexBuffer;
	delete m_indexBuffer;
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
}

//------------------------------------------------------------------------------
void Model::Render() const
{
	g_engine->m_render->DrawIndexBuffer( m_vertexBuffer, m_indexBuffer, (unsigned int)m_indexes.size() );
}

//------------------------------------------------------------------------------
void Model::CreateBuffersAndCopy()
{
	unsigned int vertexBufferSize = ( unsigned int )( m_vertexes.size() * sizeof( Vertex_PCUTBN ) );
	unsigned int vertexStride = ( unsigned int )sizeof( Vertex_PCUTBN );
	m_vertexBuffer = g_engine->m_render->CreateVertexBuffer( vertexBufferSize, vertexStride );

	unsigned int indexBufferSize = ( unsigned int )( m_indexes.size() * sizeof( unsigned int ) );
	m_indexBuffer = g_engine->m_render->CreateIndexBuffer( indexBufferSize );

	g_engine->m_render->CopyCPUToGPU( m_vertexes.data(), vertexBufferSize, m_vertexBuffer );
	g_engine->m_render->CopyCPUToGPU( m_indexes.data(), indexBufferSize, m_indexBuffer );
}