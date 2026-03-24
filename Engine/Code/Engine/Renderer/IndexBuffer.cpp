#include "Engine/Renderer/IndexBuffer.hpp"

IndexBuffer::IndexBuffer( unsigned int size )
	: m_size( size )
{

}

IndexBuffer::~IndexBuffer()
{

}

unsigned int IndexBuffer::GetSize()
{
	return m_size;
}

unsigned int IndexBuffer::GetStride()
{
	return 0;
}

unsigned int IndexBuffer::GetCount()
{
	return 0; // #TODO Index Buffer
}

