#include "Engine/Renderer/IndexBuffer.hpp"

IndexBuffer::IndexBuffer( unsigned int size )
	: m_size( size )
{

}

IndexBuffer::~IndexBuffer()
{
	DX_SAFE_RELEASE( m_buffer );
}

unsigned int IndexBuffer::GetSize()
{
	return m_size;
}

unsigned int IndexBuffer::GetStride()
{
	return sizeof( unsigned int );
}

unsigned int IndexBuffer::GetCount()
{
	return m_size / sizeof( unsigned int );
}