#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Renderer/DefaultShader.hpp"
#include "Engine/Core/Image.hpp"
#include <Engine/Core/ErrorWarningAssert.hpp>

#if defined(_DEBUG)
#define ENGINE_DEBUG_RENDER
#endif

#if defined(ENGINE_DEBUG_RENDER)
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif
#include "ThirdParty/stb/stb_image.h"

#if defined(ENGINE_DEBUG_RENDER)
void* m_dxgiDebug = nullptr;
void* m_dxgiDebugModule = nullptr;
#endif

#pragma comment(lib, "opengl32")

HGLRC g_openGLRenderingContext = nullptr;

//------------------------------------------------------------------------------
// Camera constants
//------------------------------------------------------------------------------
struct CameraConstants
{
	Mat44 WorldToCameraTransform;
	Mat44 CameraToRenderTransform;
	Mat44 RenderToClipTransform;
};

static const int k_cameraConstantsSlot = 2;

//------------------------------------------------------------------------------
// Public
//------------------------------------------------------------------------------
Renderer::Renderer( RenderConfig const& config )
	: m_config( config )
{
}

//------------------------------------------------------------------------------
Renderer::~Renderer()
{
}

//------------------------------------------------------------------------------
void Renderer::Startup()
{

	m_desiredBlendMode = BlendMode::ALPHA;
	m_desiredSamplerMode = SamplerMode::POINT_CLAMP;
	//RasterizerMode m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	//DepthMode m_desiredDepthMode = DepthMode::READ_WRITE_LESS_EQUAL;

	CreateDeviceAndSwapChain();
	CreateAndBindShaders();
	CreateBuffers();
	CreateNewRasterizerState();
	CreateBlendAndSamplerStates();
}

//------------------------------------------------------------------------------
void Renderer::Shutdown()
{
	DeleteReleaseAll();
	EngineDubugRenderer();
}

//------------------------------------------------------------------------------
void Renderer::BeginFrame()
{
	m_deviceContext->OMSetRenderTargets( 1, &m_renderTargetView, nullptr );
	BindTexture( nullptr );
}

//------------------------------------------------------------------------------
void Renderer::EndFrame()
{
	HRESULT hr = m_swapChain->Present( 0, 0 );
	if ( hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET )
	{
		ERROR_AND_DIE( "Device has been lost, application will now terminate." );
	}
}

//------------------------------------------------------------------------------
void Renderer::ClearScreen( Rgba8 const& clearColor )
{
	float colorAsFloats[4];
	clearColor.GetAsFloats( colorAsFloats );
	m_deviceContext->ClearRenderTargetView( m_renderTargetView, colorAsFloats );
}

//------------------------------------------------------------------------------
void Renderer::BeginCamera( Camera const& camera )
{
	D3D11_VIEWPORT viewport = { };
	viewport.Width = ( float )g_engine->m_window->GetClientDimensions().x;
	viewport.Height = ( float )g_engine->m_window->GetClientDimensions().y;
	viewport.MaxDepth = 1.0f;
	m_deviceContext->RSSetViewports( 1, &viewport );

	CameraConstants camConstants = { };
	camConstants.WorldToCameraTransform = camera.GetWorldToCameraTransform();
	camConstants.CameraToRenderTransform = camera.GetCameraToRenderTransform();
	camConstants.RenderToClipTransform = camera.GetRenderToClipTransform();

	CopyCPUToGPU( &camConstants, sizeof( camConstants ), m_cameraCBO );
	BindConstantBuffer( k_cameraConstantsSlot, m_cameraCBO );
}

//------------------------------------------------------------------------------
void Renderer::EndCamera( Camera const& )
{
}

//------------------------------------------------------------------------------
void Renderer::DrawVertexArray( int numVertexes, Vertex const* vertexes )
{
	CopyCPUToGPU( vertexes, numVertexes * sizeof( Vertex ), m_immediateVBO );
	DrawVertexBuffer( m_immediateVBO, numVertexes );
}

//------------------------------------------------------------------------------
void Renderer::DrawVertexArray( std::vector<Vertex> const& verts )
{
	DrawVertexArray( ( int )verts.size(), verts.data() );
}

//------------------------------------------------------------------------------
// Private
//------------------------------------------------------------------------------
Shader* Renderer::CreateShader( char const* shaderName, char const* shaderSource )
{
	ShaderConfig config;
	config.m_name = shaderName;
	Shader* shader = new Shader( config );

	std::vector<uint8_t> vertexBytes;
	CompileShaderToByteCode( vertexBytes, config.m_name.c_str(), shaderSource, config.m_vertexEntryPoint.c_str(), "vs_5_0" );

	m_device->CreateVertexShader( vertexBytes.data(), vertexBytes.size(), NULL, &shader->m_vertexShader );

	std::vector<uint8_t> pixelBytes;
	CompileShaderToByteCode( pixelBytes, config.m_name.c_str(), shaderSource, config.m_pixelEntryPoint.c_str(), "ps_5_0" );
	m_device->CreatePixelShader( pixelBytes.data(), pixelBytes.size(), NULL, &shader->m_pixelShader );

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	m_device->CreateInputLayout(
		inputElementDesc,
		ARRAYSIZE( inputElementDesc ),
		vertexBytes.data(),
		vertexBytes.size(),
		&shader->m_inputLayout
	);

	return shader;
}

//------------------------------------------------------------------------------
Shader* Renderer::CreateShader( char const* shaderName )
{
	std::string fileName = std::string( shaderName ) + ".hlsl";
	std::string outString;
	FileReadToString( outString, fileName );
	return CreateShader( shaderName, outString.c_str() );
}

//------------------------------------------------------------------------------
void Renderer::BindShader( Shader* shader )
{
	if ( shader == nullptr )
	{
		shader = m_defaultShader;
	}
	m_deviceContext->VSSetShader( shader->m_vertexShader, nullptr, 0 );
	m_deviceContext->PSSetShader( shader->m_pixelShader, nullptr, 0 );
	m_deviceContext->IASetInputLayout( shader->m_inputLayout );
}

//------------------------------------------------------------------------------
bool Renderer::CompileShaderToByteCode(
	std::vector<unsigned char>& outByteCode,
	char const* name,
	char const* source,
	char const* entryPoint,
	char const* target )
{
	DWORD shaderFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#if defined(ENGINE_DEBUG_RENDER)
	shaderFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_WARNINGS_ARE_ERRORS;
#endif

	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	HRESULT hr = D3DCompile(
		source, strlen( source ), name,
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint, target, shaderFlags, 0,
		&shaderBlob, &errorBlob
	);

	if ( FAILED( hr ) )
	{
		if ( errorBlob )
		{
			errorBlob->Release();
			errorBlob = nullptr;

		}
		ERROR_AND_DIE( "Shader compilation failed" );
	}

	if ( SUCCEEDED( hr ) )
	{
		outByteCode.resize( shaderBlob->GetBufferSize() );
		memcpy( outByteCode.data(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize() );
	}

	if ( shaderBlob ) shaderBlob->Release();
	if ( errorBlob ) errorBlob->Release();

	return true;
}

//------------------------------------------------------------------------------
ConstantBuffer* Renderer::CreateConstantBuffer( const unsigned int size )
{
	D3D11_BUFFER_DESC bufferDesc = { };
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = ( UINT )size;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	ConstantBuffer* buffer = new ConstantBuffer( size );
	m_device->CreateBuffer( &bufferDesc, nullptr, &buffer->m_buffer );
	return buffer;
}

//------------------------------------------------------------------------------
void Renderer::BindConstantBuffer( int slot, ConstantBuffer* cbo )
{
	m_deviceContext->VSSetConstantBuffers( slot, 1, &cbo->m_buffer );
	m_deviceContext->PSSetConstantBuffers( slot, 1, &cbo->m_buffer );
}

//------------------------------------------------------------------------------
VertexBuffer* Renderer::CreateVertexBuffer( const unsigned int size, unsigned int stride )
{
	return new VertexBuffer( m_device, size, stride );
}

//------------------------------------------------------------------------------
void Renderer::BindVertexBuffer( VertexBuffer* vbo )
{
	UINT offset = 0;
	m_deviceContext->IASetVertexBuffers( 0, 1, &vbo->m_buffer, &vbo->m_stride, &offset );
	m_deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}

//------------------------------------------------------------------------------
void Renderer::DrawVertexBuffer( VertexBuffer* vbo, unsigned int vertexCount )
{
	SetStatesIfChanged();
	SetSamplerMode( m_desiredSamplerMode );
	BindVertexBuffer( vbo );
	m_deviceContext->Draw( vertexCount, 0 );
}

//------------------------------------------------------------------------------
void Renderer::CopyCPUToGPU( const void* data, unsigned int size, VertexBuffer* vbo )
{
	if ( vbo->GetSize() < size )
	{
		vbo->Resize( size );
	}

	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceContext->Map( vbo->m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource );
	memcpy( resource.pData, data, size );
	m_deviceContext->Unmap( vbo->m_buffer, 0 );
}

//------------------------------------------------------------------------------
void Renderer::CopyCPUToGPU( const void* data, unsigned int size, ConstantBuffer* cbo )
{
	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceContext->Map( cbo->m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource );
	memcpy( resource.pData, data, size );
	m_deviceContext->Unmap( cbo->m_buffer, 0 );
}

//------------------------------------------------------------------------------
void Renderer::SetBlendMode( BlendMode mode )
{
	m_desiredBlendMode = mode;
}

//------------------------------------------------------------------------------
void Renderer::SetStatesIfChanged()
{
	ID3D11BlendState* desiredState = m_blendStates[(int)m_desiredBlendMode];
	if ( desiredState != m_blendState )
	{
		m_blendState = desiredState;
		float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		UINT sampleMask = 0xffffffff;
		m_deviceContext->OMSetBlendState( m_blendState, blendFactor, sampleMask );
	}
}

//------------------------------------------------------------------------------
void Renderer::CreateBlendStates( D3D11_BLEND sourceBlend, D3D11_BLEND destBlend, BlendMode mode )
{
	D3D11_BLEND_DESC blendDesc = { };
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = sourceBlend;
	blendDesc.RenderTarget[0].DestBlend = destBlend;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = blendDesc.RenderTarget[0].SrcBlend;
	blendDesc.RenderTarget[0].DestBlendAlpha = blendDesc.RenderTarget[0].DestBlend;
	blendDesc.RenderTarget[0].BlendOpAlpha = blendDesc.RenderTarget[0].BlendOp;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HRESULT hr;
	hr = m_device->CreateBlendState(
		&blendDesc,
		&m_blendStates[( int )( mode )]
	);

	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "CreateBlendState for BlendMode::OPAQUE failed." );
	}
}

//------------------------------------------------------------------------------
void Renderer::DeleteReleaseAll()
{
	DX_SAFE_RELEASE( m_rasterizerState );
	DX_SAFE_RELEASE( m_renderTargetView );
	DX_SAFE_RELEASE( m_swapChain );
	DX_SAFE_RELEASE( m_deviceContext );
	DX_SAFE_RELEASE( m_device );

	for ( int i = 0; i < m_loadedShaders.size(); i++ )
	{
		delete m_loadedShaders[i];
		m_loadedShaders[i] = nullptr;
	}
	m_loadedShaders.clear();

	for ( int i = 0; i < m_loadedTextures.size(); i++ )
	{
		delete m_loadedTextures[i];
		m_loadedTextures[i] = nullptr;
	}
	m_loadedTextures.clear();

	delete m_immediateVBO;
	m_immediateVBO = nullptr;

	delete m_cameraCBO;
	m_cameraCBO = nullptr;

	for ( int i = 0; i < ( int )BlendMode::COUNT; ++i )
	{
		DX_SAFE_RELEASE( m_blendStates[i] );
	}

	for ( int i = 0; i < ( int )SamplerMode::COUNT; ++i )
	{
		DX_SAFE_RELEASE( m_samplerStates[i] );
	}
}

//------------------------------------------------------------------------------
void Renderer::EngineDubugRenderer()
{
#if defined(ENGINE_DEBUG_RENDER)
	m_dxgiDebugModule = ( void* )::LoadLibraryA( "dxgidebug.dll" );
	typedef HRESULT( WINAPI* GetDebugModuleCB )( REFIID, void** );
	( ( GetDebugModuleCB )::GetProcAddress( ( HMODULE )m_dxgiDebugModule, "DXGIGetDebugInterface" ) )
		( _uuidof( IDXGIDebug ), &m_dxgiDebug );

	( ( IDXGIDebug* )m_dxgiDebug )->ReportLiveObjects(
		DXGI_DEBUG_ALL,
		( DXGI_DEBUG_RLO_FLAGS )( DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL )
	);
	( ( IDXGIDebug* )m_dxgiDebug )->Release();
	::FreeLibrary( ( HMODULE )m_dxgiDebugModule );
#endif
}

//------------------------------------------------------------------------------
void Renderer::CreateBuffers()
{
	m_immediateVBO = CreateVertexBuffer( sizeof( Vertex ), sizeof( Vertex ) );
	m_cameraCBO = CreateConstantBuffer( sizeof( CameraConstants ) );
}

//------------------------------------------------------------------------------
void Renderer::CreateDeviceAndSwapChain()
{
	unsigned int deviceFlags = 0;
#if defined(ENGINE_DEBUG_RENDER)
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	DXGI_SWAP_CHAIN_DESC swapChainDesc = { };
	swapChainDesc.BufferDesc.Width = g_engine->m_window->GetClientDimensions().x;
	swapChainDesc.BufferDesc.Height = g_engine->m_window->GetClientDimensions().y;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = ( HWND )g_engine->m_window->GetHwnd();
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, deviceFlags,
		nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc,
		&m_swapChain, &m_device, nullptr, &m_deviceContext );

	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "Could not create D3D 11 device and swap chain." );
	}
	ID3D11Texture2D* backBuffer;
	hr = m_swapChain->GetBuffer( 0, _uuidof( ID3D11Texture2D ), ( void** )&backBuffer );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "Could not get swap chain buffer." );
	}

	hr = m_device->CreateRenderTargetView( backBuffer, NULL, &m_renderTargetView );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "Could create render target view for swap chain buffer." );
	}
	backBuffer->Release();
}

//------------------------------------------------------------------------------
void Renderer::CreateNewRasterizerState()
{
	D3D11_RASTERIZER_DESC rasterizerDesc = { };
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.AntialiasedLineEnable = true;

	HRESULT hr;
	hr = m_device->CreateRasterizerState( &rasterizerDesc, &m_rasterizerState );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "Could not create rasterizer state." );
	}
	m_deviceContext->RSSetState( m_rasterizerState );
}

//-----------------------------------------------------------------------------------------------
Texture* Renderer::CreateTextureFromFile( const char* imageFilePath )
{
	Image imageFromFile = Image( imageFilePath );
	return CreateTextureFromImage( imageFromFile );
}

//-----------------------------------------------------------------------------------------------
Texture* Renderer::CreateTextureFromImage( const Image& image )
{
	Texture* newTexture = new Texture();
	//newTexture->m_name = "Default";
	newTexture->m_name = image.GetImageFilePath(); // NOTE: m_name must be a std::string, otherwise it may point to temporary data!
	newTexture->m_dimensions = image.GetDimensions();
	//Texture* newTexture = CreateTextureFromFile( image.GetImageFilePath().c_str() );

	D3D11_TEXTURE2D_DESC textureDesc = { };
	textureDesc.Width = image.GetDimensions().x;
	textureDesc.Height = image.GetDimensions().y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA textureData;
	textureData.pSysMem = image.GetRawData();
	textureData.SysMemPitch = 4 * image.GetDimensions().x;

	HRESULT hr;
	hr = m_device->CreateTexture2D( &textureDesc, &textureData, &newTexture->m_texture );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( Stringf( "CreateTextureFromImage failed for image file \"%s\".",
			image.GetImageFilePath().c_str() ) );
	}

	hr = m_device->CreateShaderResourceView( newTexture->m_texture, NULL,
		&newTexture->m_shaderResourceView );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( Stringf( "CreateShaderResourceView failed for image file \"%s\".",
			image.GetImageFilePath().c_str() ) );
	}

	m_loadedTextures.push_back( newTexture );
	return newTexture;
}

//-----------------------------------------------------------------------------------------------
void Renderer::BindTexture( Texture* texture )
{
	if ( texture == nullptr )
	{
		m_deviceContext->PSSetShaderResources( 0, 1, &m_defaultTexture->m_shaderResourceView );
	}
	else
	{
		m_currentTexture = texture;
		m_deviceContext->PSSetShaderResources( 0, 1, &m_currentTexture->m_shaderResourceView );
	}
}

//------------------------------------------------------------------------------------------------
Texture* Renderer::CreateOrGetTextureFromFile( char const* imageFilePath )
{
	Texture* existingTexture = GetTextureForFileName( imageFilePath );
	if ( existingTexture )
	{
		return existingTexture;
	}

	// Never seen this texture before!  Let's load it.
	Texture* newTexture = CreateTextureFromFile( imageFilePath );
	return newTexture;
}

//-----------------------------------------------------------------------------------------------
Texture* Renderer::GetTextureForFileName( char const* imageFilePath )
{
	for ( int textureIndex = 0; textureIndex < static_cast< int >( m_loadedTextures.size() ); ++textureIndex )
	{
		Texture* texture = m_loadedTextures[textureIndex];
		if ( texture->m_name == imageFilePath )
		{
			return texture;
		}
	}
	return nullptr;
}

//-----------------------------------------------------------------------------------------------
BitmapFont* Renderer::CreateOrGetBitmapFont( char const* bitmapFontFilePathWithNoExtension )
{
	// See if we already have this font previously loaded
	BitmapFont* existingBitmapFont = GetFontForFileName( bitmapFontFilePathWithNoExtension );
	if ( existingBitmapFont )
	{
		return existingBitmapFont;
	}

	// Never seen this texture before!  Let's load it.
	BitmapFont* newFont = CreateFontFromFile( bitmapFontFilePathWithNoExtension );
	return newFont;
}

//-----------------------------------------------------------------------------------------------
BitmapFont* Renderer::GetFontForFileName( char const* bitmapFontFilePathWithNoExtension )
{
	for ( int fontIndex = 0; fontIndex < static_cast< int >( m_loadedFonts.size() ); ++fontIndex )
	{
		BitmapFont* font = m_loadedFonts[fontIndex];
		if ( font->m_fontFilePathNameWithNoExtension == bitmapFontFilePathWithNoExtension )
		{
			return font;
		}
	}
	return nullptr;
}

//-----------------------------------------------------------------------------------------------
BitmapFont* Renderer::CreateFontFromFile( char const* bitmapFontFilePathWithNoExtension )
{
	std::string fontTextureFilePath = bitmapFontFilePathWithNoExtension;
	fontTextureFilePath += ".png";
	Texture* fontTexture = CreateOrGetTextureFromFile( fontTextureFilePath.c_str() );
	BitmapFont* newFont = new BitmapFont( bitmapFontFilePathWithNoExtension, *fontTexture );
	m_loadedFonts.push_back( newFont );
	return newFont;
}

//-----------------------------------------------------------------------------------------------
void Renderer::CreateSamplerMode( SamplerMode mode, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressU, D3D11_TEXTURE_ADDRESS_MODE addressV, D3D11_TEXTURE_ADDRESS_MODE addressW )
{
	D3D11_SAMPLER_DESC samplerDesc = { };
	samplerDesc.Filter = filter;
	samplerDesc.AddressU = addressU;
	samplerDesc.AddressV = addressV;
	samplerDesc.AddressW = addressW;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT hr;
	hr = m_device->CreateSamplerState( &samplerDesc,
		&m_samplerStates[( int ) mode ] );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "CreateSamplerState for SamplerMode :: POINT_CLAMP failed." );

	}
}

//------------------------------------------------------------------------------
void Renderer::CreateBlendAndSamplerStates()
{
	CreateBlendStates( D3D11_BLEND_ONE, D3D11_BLEND_ZERO, BlendMode::OPAQUE );
	CreateBlendStates( D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, BlendMode::ALPHA );
	CreateBlendStates( D3D11_BLEND_ONE, D3D11_BLEND_ONE, BlendMode::ADDITIVE );

	Image whiteImage = Image( IntVec2( 2, 2 ), Rgba8( 255, 255, 255, 255 ) );
	m_defaultTexture = CreateTextureFromImage( whiteImage );
	BindTexture( const_cast< Texture* >( m_defaultTexture ) );

	CreateSamplerMode( SamplerMode::POINT_CLAMP, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP ); // Point clamp
	CreateSamplerMode( SamplerMode::BILINEAR_WRAP, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP ); // Bilinear wrap
}

//------------------------------------------------------------------------------
void Renderer::CreateAndBindShaders()
{
	m_currentShader = CreateShader( "Current", defaultShaderSource );
	m_defaultShader = CreateShader( "Default", defaultShaderSource );
	m_loadedShaders.push_back( m_currentShader );
	m_loadedShaders.push_back( m_defaultShader );
	BindShader( m_currentShader );
}

//-----------------------------------------------------------------------------------------------
void Renderer::SetSamplerMode( SamplerMode mode )
{
	if ( m_samplerStates[( int )mode] != m_samplerState )
	{
		m_samplerState = m_samplerStates[( int )mode];
		m_deviceContext->PSSetSamplers( 0, 1, &m_samplerState );
	}
}