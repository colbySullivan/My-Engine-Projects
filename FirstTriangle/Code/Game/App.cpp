#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "App.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"  
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include <Engine/Core/ErrorWarningAssert.hpp>

#if defined(_DEBUG)
#define ENGINE_DEBUG_RENDER
#endif

#if defined(ENGINE_DEBUG_RENDER)
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

ID3D11Device* m_device = nullptr;
ID3D11DeviceContext* m_deviceContext = nullptr;
IDXGISwapChain* m_swapChain = nullptr;
ID3D11RenderTargetView* m_renderTargetView = nullptr;
ID3D11VertexShader* m_vertexShader = nullptr;
ID3D11PixelShader* m_pixelShader = nullptr;
ID3D11InputLayout* m_inputLayoutForVertex_PCU = nullptr;
ID3D11Buffer* m_vertexBuffer = nullptr;
ID3D11RasterizerState* m_rasterizerState = nullptr;

std::vector<uint8_t> m_vertexShaderByteCode;
std::vector<uint8_t> m_pixelShaderByteCode;

#if defined(ENGINE_DEBUG_RENDER)
void* m_dxgiDebug = nullptr;
void* m_dxgiDebugModule = nullptr;
#endif

// Draw triangles
const char* shaderSource = R"(
	struct vs_input_t
	{
		float3 localPosition : POSITION;
		float4 color : COLOR;
		float2 uv : TEXCOORD;
	};
	struct v2p_t
	{
		float4 position : SV_Position;
		float4 color : COLOR;
		float2 uv : TEXCOORD;
	};
	v2p_t VertexMain(vs_input_t input)
	{
		v2p_t v2p;
		v2p.position = float4(input.localPosition, 1);
		v2p.color = input.color;
		v2p.uv = input.uv;
		return v2p;
	};
	float4 PixelMain(v2p_t input) : SV_Target0
	{
		return float4(input.color);
	};
	)";


App* g_app = nullptr;

App::App()
{
	EngineConfig config;
	config.m_windowConfig.m_clientAspect = 2.0f;
	config.m_windowConfig.m_windowTitle = "FirstTriangle";
	g_engine = new Engine( config );
	g_app = this;

	//CREATE THE DEVICE AND SWAP CHAIN
	unsigned int deviceFlags = 0;
#if defined(ENGINE_DEBUG_RENDER)
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Create device and swap chain
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

	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, deviceFlags,
		nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc,
		&m_swapChain, &m_device, nullptr, &m_deviceContext );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "Could not create D3D 11 device and swap chain." );
	}

	// SAVE BACK BUFFER VIEW
	// Get back buffer texture
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

	// SHUTDOWN AND CHECK FOR MEMORY LEAKS

	// Create debug module
#if defined(ENGINE_DEBUG_RENDER)
	m_dxgiDebugModule = ( void* ) ::LoadLibraryA( "dxgidebug.dll" );
	if ( m_dxgiDebugModule == nullptr )
	{
		ERROR_AND_DIE( "Could not load dxgidebug.dll." );
	}

	typedef HRESULT( WINAPI* GetDebugModuleCB )( REFIID, void** );
	( ( GetDebugModuleCB ) ::GetProcAddress( ( HMODULE )m_dxgiDebugModule, "DXGIGetDebugInterface" ) )
	( _uuidof( IDXGIDebug ), &m_dxgiDebug );

	if ( m_dxgiDebug == nullptr )
	{
		ERROR_AND_DIE( "Could not load debug module." );
	}
#endif


	// Compile vertex shader
	DWORD shaderFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#if defined(ENGINE_DEBUG_RENDER)
	shaderFlags = D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	shaderFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
#endif
	ID3DBlob* shaderBlob = NULL;
	ID3DBlob* errorBlob = NULL;

	hr = D3DCompile(
		shaderSource, strlen( shaderSource ),
		"VertexShader", nullptr, nullptr,
		"VertexMain", "vs_5_0", shaderFlags,
		0, &shaderBlob, &errorBlob );
	if ( SUCCEEDED( hr ) )
	{
		m_vertexShaderByteCode.resize( shaderBlob->GetBufferSize() );
		memcpy(
			m_vertexShaderByteCode.data(),
			shaderBlob->GetBufferPointer(),
			shaderBlob->GetBufferSize() );
	}
	else
	{
		if ( errorBlob != NULL )
		{
			DebuggerPrintf( ( char* )errorBlob->GetBufferPointer() );
		}
		ERROR_AND_DIE( Stringf( "Could not compile vertex shader." ) );
	}

	shaderBlob->Release();
	if ( errorBlob != NULL )
	{
		errorBlob->Release();
	}

	// Create vertex shader
	hr = m_device->CreateVertexShader(
		m_vertexShaderByteCode.data(),
		m_vertexShaderByteCode.size(),
		NULL, &m_vertexShader
	);
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( Stringf( "Could not create vertex shader." ) );
	}

	// Compile pixel shader
	hr = D3DCompile(
		shaderSource, strlen( shaderSource ),
		"PixelShader", nullptr, nullptr,
		"PixelMain", "ps_5_0", shaderFlags, 0,
		&shaderBlob, &errorBlob
	);
	if ( SUCCEEDED( hr ) )
	{
		m_pixelShaderByteCode.resize( shaderBlob->GetBufferSize() );
		memcpy(
			m_pixelShaderByteCode.data(),
			shaderBlob->GetBufferPointer(),
			shaderBlob->GetBufferSize()
		);
	}
	else
	{
		if ( errorBlob != NULL )
		{
			DebuggerPrintf( ( char* )errorBlob->GetBufferPointer() );
		}
		ERROR_AND_DIE( Stringf( "Could not compile pixel shader." ) );
	}

	shaderBlob->Release();
	if ( errorBlob != NULL )
	{
		errorBlob->Release();

	}

	// Create pixel shader
	hr = m_device->CreatePixelShader(
		m_pixelShaderByteCode.data(),
		m_pixelShaderByteCode.size(),
		NULL, &m_pixelShader );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( Stringf( "Could not create pixel shader." ) );
	}

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM,
		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	UINT numElements = ARRAYSIZE( inputElementDesc );
	hr = m_device->CreateInputLayout(
		inputElementDesc, numElements,
		m_vertexShaderByteCode.data(),
		m_vertexShaderByteCode.size(),
		&m_inputLayoutForVertex_PCU
	);
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "Could not create vertex layout." );
	}

	Vertex vertices[] = {
		Vertex( Vec3( -0.50f, -0.50f, 0.0f ), Rgba8( 255, 255, 255, 255 ), Vec2( 0.0f, 0.0f ) ),
		Vertex( Vec3( 0.00f, 0.50f, 0.0f ), Rgba8( 255, 255, 255, 255 ), Vec2( 0.0f, 0.0f ) ),
		Vertex( Vec3( 0.50f, -0.50f, 0.0f ), Rgba8( 255, 255, 255,255 ), Vec2( 0.0f, 0.0f ) ),
	};

	// Create vertex buffer
	UINT vertexBufferSize = ( UINT )sizeof( vertices );
	D3D11_BUFFER_DESC bufferDesc = { };
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = vertexBufferSize;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = m_device->CreateBuffer( &bufferDesc, nullptr, &m_vertexBuffer );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "Could not create vertex buffer." );
	}

	// Copy vertices
	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceContext->Map( m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource );
	memcpy( resource.pData, vertices, vertexBufferSize );
	m_deviceContext->Unmap( m_vertexBuffer, 0 );

	// Set viewport
	D3D11_VIEWPORT viewport = { };
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = ( float )g_engine->m_window->GetClientDimensions().x;
	viewport.Height = ( float )g_engine->m_window->GetClientDimensions().y;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	m_deviceContext->RSSetViewports( 1, &viewport );

	// Set rasterizer state
	D3D11_RASTERIZER_DESC rasterizerDesc = { };
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = true;

	hr = m_device->CreateRasterizerState( &rasterizerDesc, &m_rasterizerState );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "Could not create rasterizer state." );
	}

	m_deviceContext->RSSetState( m_rasterizerState );

	UINT stride = sizeof( Vertex );
	UINT startOffset = 0;
	m_deviceContext->IASetVertexBuffers( 0, 1, &m_vertexBuffer, &stride, &startOffset );
	m_deviceContext->IASetInputLayout( m_inputLayoutForVertex_PCU );
	m_deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	m_deviceContext->VSSetShader( m_vertexShader, nullptr, 0 );
	m_deviceContext->PSSetShader( m_pixelShader, nullptr, 0 );
}
//-----------------------------------------------------------------------------------------------

App::~App()
{
	m_rasterizerState->Release();
	m_vertexBuffer->Release();
	m_vertexShader->Release();
	m_pixelShader->Release();
	m_inputLayoutForVertex_PCU->Release();
	m_renderTargetView->Release();
	m_swapChain->Release();
	m_deviceContext->Release();
	m_device->Release();

	// Report error leaks and release debug module
#if defined(ENGINE_DEBUG_RENDER)
	( ( IDXGIDebug* )m_dxgiDebug )->ReportLiveObjects(
		DXGI_DEBUG_ALL,
		( DXGI_DEBUG_RLO_FLAGS )( DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL )
	);
		( ( IDXGIDebug* )m_dxgiDebug )->Release();
	m_dxgiDebug = nullptr;

	::FreeLibrary( ( HMODULE )m_dxgiDebugModule );
	m_dxgiDebugModule = nullptr;
#endif
}
//-----------------------------------------------------------------------------------------------

void App::RunFrame()
{
	float timeNow = (float)GetCurrentTimeSeconds();
	float deltaSeconds = timeNow - m_lastFrameTime;
	m_lastFrameTime = timeNow;

	g_engine->BeginFrame();
	Update(deltaSeconds);
	Render();
}
//-----------------------------------------------------------------------------------------------

void App::Update([[maybe_unused]] float deltaSeconds)
{
	if ( g_engine->m_input->IsKeyDown( KEYCODE_ESC ) )
	{
		SetIsQuitting();
	}
}
//-----------------------------------------------------------------------------------------------

void App::Render() const
{
	// Set the render target
	m_deviceContext->OMSetRenderTargets( 1, &m_renderTargetView, nullptr );

	// Clear the screen
	Rgba8 clearColor( 127, 127, 127, 255 );
	float colorAsFloats[4];
	clearColor.GetAsFloats( colorAsFloats );
	m_deviceContext->ClearRenderTargetView( m_renderTargetView, colorAsFloats );

	// Draw
	m_deviceContext->Draw( 3, 0 );

	// Present
	HRESULT hr;
	hr = m_swapChain->Present( 0, 0 );
	if ( hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET )
	{ 
		ERROR_AND_DIE( "Device has been lost, application will now terminate." );
	}
}

//-----------------------------------------------------------------------------------------------

void App::SetIsQuitting()
{
	m_isQuitting = true;
}
//-----------------------------------------------------------------------------------------------

bool App::IsQuitting() const
{
	return m_isQuitting;
}

