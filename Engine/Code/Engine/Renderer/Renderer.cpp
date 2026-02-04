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
#include <Engine/Core/ErrorWarningAssert.hpp>

#if defined(_DEBUG)
#define ENGINE_DEBUG_RENDER
#endif

#if defined(ENGINE_DEBUG_RENDER)
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

#if defined(ENGINE_DEBUG_RENDER)
void* m_dxgiDebug = nullptr;
void* m_dxgiDebugModule = nullptr;
#endif

#pragma comment(lib, "opengl32")

HGLRC g_openGLRenderingContext = nullptr; 

Renderer::Renderer( RenderConfig const& config )
	: m_config( config )
{

}

Renderer::~Renderer()
{

}

void Renderer::Startup()
{
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
	swapChainDesc.OutputWindow = (HWND)g_engine->m_window->GetHwnd();
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, deviceFlags,
		nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc,
		&m_swapChain, &m_device, nullptr, &m_deviceContext);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create D3D 11 device and swap chain.");
	}

	// SAVE BACK BUFFER VIEW
	// Get back buffer texture
	ID3D11Texture2D* backBuffer;
	hr = m_swapChain->GetBuffer(0, _uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not get swap chain buffer.");
	}

	hr = m_device->CreateRenderTargetView(backBuffer, NULL, &m_renderTargetView);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could create render target view for swap chain buffer.");
	}
	backBuffer->Release();
	m_currentShader = CreateShader("Default", defaultShaderSource);
	m_defaultShader = CreateShader("Default", defaultShaderSource);
	m_loadedShaders.push_back(m_currentShader);
	BindShader(m_currentShader);

	CreateVertexBuffer(sizeof(Vertex), sizeof(Vertex));
	
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

	hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create rasterizer state.");
	}
	m_deviceContext->RSSetState(m_rasterizerState);
}

void Renderer::Shutdown()
{
	DX_SAFE_RELEASE(m_rasterizerState);
	DX_SAFE_RELEASE(m_renderTargetView);
	DX_SAFE_RELEASE(m_swapChain);
	DX_SAFE_RELEASE(m_deviceContext);
	DX_SAFE_RELEASE(m_device);
	
	// Delete all shaders in cache
	for (int i = 0; i < m_loadedShaders.size(); i++)
	{
		if (m_loadedShaders[i] != nullptr)
		{
			delete m_loadedShaders[i];
			m_loadedShaders[i] = nullptr;
		}
	}
	m_loadedShaders.clear();
	m_currentShader = nullptr;

	delete m_immediateVBO;
	m_immediateVBO = nullptr;

	// Create debug module
#if defined(ENGINE_DEBUG_RENDER)
	m_dxgiDebugModule = (void*) ::LoadLibraryA("dxgidebug.dll");
	if (m_dxgiDebugModule == nullptr)
	{
		ERROR_AND_DIE("Could not load dxgidebug.dll.");
	}

	typedef HRESULT(WINAPI* GetDebugModuleCB)(REFIID, void**);
	((GetDebugModuleCB) ::GetProcAddress((HMODULE)m_dxgiDebugModule, "DXGIGetDebugInterface"))
		(_uuidof(IDXGIDebug), &m_dxgiDebug);

	if (m_dxgiDebug == nullptr)
	{
		ERROR_AND_DIE("Could not load debug module.");
	}
#endif
	// Report error leaks and release debug module
#if defined(ENGINE_DEBUG_RENDER)
	((IDXGIDebug*)m_dxgiDebug)->ReportLiveObjects(
		DXGI_DEBUG_ALL,
		(DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL)
	);
	((IDXGIDebug*)m_dxgiDebug)->Release();
	m_dxgiDebug = nullptr;

	::FreeLibrary((HMODULE)m_dxgiDebugModule);
	m_dxgiDebugModule = nullptr;
#endif
}

void Renderer::BeginFrame()
{
	// Set the render target
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, nullptr);
}

void Renderer::EndFrame()
{
	// Present
	HRESULT hr;
	hr = m_swapChain->Present(0, 0);
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		ERROR_AND_DIE("Device has been lost, application will now terminate.");
	}
}

void Renderer::ClearScreen(Rgba8 const& clearColor)
{
	float colorAsFloats[4];
	clearColor.GetAsFloats(colorAsFloats);
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, colorAsFloats);
}

void Renderer::BeginCamera( [[maybe_unused]] Camera const& camera)
{
	// Set viewport
	D3D11_VIEWPORT viewport = { };
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = (float)g_engine->m_window->GetClientDimensions().x;
	viewport.Height = (float)g_engine->m_window->GetClientDimensions().y;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	m_deviceContext->RSSetViewports(1, &viewport);
}

void Renderer::EndCamera( [[maybe_unused]] Camera const& camera)
{

}

void Renderer::DrawVertexArray( std::vector<Vertex> const& verts )
{
	DrawVertexArray( static_cast< int >( verts.size() ), verts.data() );
}

//------------------------------------------------------------------------------
Shader* Renderer::CreateShader(char const* shaderName, char const* shaderSource)
{
	ShaderConfig config;
	config.m_name = shaderName;
	Shader* shader = new Shader(config);

	std::vector<uint8_t> vertexBytes;
	CompileShaderToByteCode(vertexBytes, config.m_name.c_str(), shaderSource, config.m_vertexEntryPoint.c_str(), "vs_5_0");
	
	// Create vertex shader
	HRESULT hr;
	hr = m_device->CreateVertexShader(
		vertexBytes.data(),
		vertexBytes.size(),
		NULL, &shader->m_vertexShader
	);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("Could not create vertex shader."));
	}
	std::vector<uint8_t> pixelBytes;
	CompileShaderToByteCode(pixelBytes, config.m_name.c_str(), shaderSource, config.m_pixelEntryPoint.c_str(), "ps_5_0");
	// Create pixel shader
	hr = m_device->CreatePixelShader(
		pixelBytes.data(),
		pixelBytes.size(),
		NULL, &shader->m_pixelShader);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("Could not create pixel shader."));
	}

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM,
		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	UINT numElements = ARRAYSIZE(inputElementDesc);
	hr = m_device->CreateInputLayout(
		inputElementDesc, numElements,
		vertexBytes.data(),
		vertexBytes.size(),
		&shader->m_inputLayout
	);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create vertex layout.");
	}
	return shader;
}

//------------------------------------------------------------------------------
bool Renderer::CompileShaderToByteCode(std::vector<unsigned char>& outByteCode, char const* name, char const* source, char const* entryPoint, char const* target)
{
	// Compile vertex shader
	DWORD shaderFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#if defined(ENGINE_DEBUG_RENDER)
	shaderFlags = D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	shaderFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
#endif
	ID3DBlob* shaderBlob = NULL;
	ID3DBlob* errorBlob = NULL;

	// Compile pixel shader
	HRESULT hr;
	hr = D3DCompile(
		source, strlen(source),
		name, nullptr, nullptr,
		entryPoint, target, shaderFlags, 0,
		&shaderBlob, &errorBlob
	);
	if (SUCCEEDED(hr))
	{
		outByteCode.resize(shaderBlob->GetBufferSize());
		memcpy(
			outByteCode.data(),
			shaderBlob->GetBufferPointer(),
			shaderBlob->GetBufferSize()
		);
	}
	else
	{
		if (errorBlob != NULL)
		{
			DebuggerPrintf((char*)errorBlob->GetBufferPointer());
		}
		ERROR_AND_DIE(Stringf("Could not compile pixel shader."));
	}

	shaderBlob->Release();
	if (errorBlob != NULL)
	{
		errorBlob->Release();
	}
	return true;
}

//------------------------------------------------------------------------------
void Renderer::BindShader(Shader* shader)
{
	if ( shader == NULL )
	{
		shader = m_defaultShader;
	}
	m_deviceContext->VSSetShader(shader->m_vertexShader, nullptr, 0);
	m_deviceContext->PSSetShader(shader->m_pixelShader, nullptr, 0);
	m_deviceContext->IASetInputLayout(shader->m_inputLayout);
}

//------------------------------------------------------------------------------
VertexBuffer* Renderer::CreateVertexBuffer(const unsigned int size, unsigned int stride)
{
	m_immediateVBO = new VertexBuffer(m_device, size, stride);
	return m_immediateVBO;
}

//------------------------------------------------------------------------------
void Renderer::CopyCPUToGPU(const void* data, unsigned int size, VertexBuffer* vbo)
{
	// TODO : First check if the passed in vertex buffer is large enough to hold the data passed in, and if not, resize it.
	if ( vbo->GetSize() < size )
	{
		vbo->Resize(size);
	}
	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceContext->Map(vbo->m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, size);
	m_deviceContext->Unmap(vbo->m_buffer, 0);
}

//------------------------------------------------------------------------------
void Renderer::BindVertexBuffer(VertexBuffer* vbo)
{
	UINT startOffset = 0;
	m_deviceContext->IASetVertexBuffers(0, 1, &vbo->m_buffer, &vbo->m_stride, &startOffset);
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

//------------------------------------------------------------------------------
void Renderer::DrawVertexBuffer(VertexBuffer* vbo, unsigned int vertexCount)
{
	BindVertexBuffer(vbo);
	m_deviceContext->Draw(vertexCount, 0);
}

void Renderer::DrawVertexArray(int numVertexes, Vertex const* vertexes)
{
	CopyCPUToGPU(vertexes, numVertexes * sizeof(Vertex), m_immediateVBO);
	DrawVertexBuffer(m_immediateVBO, numVertexes);
}