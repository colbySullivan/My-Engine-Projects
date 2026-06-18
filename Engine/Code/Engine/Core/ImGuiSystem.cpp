#include "Engine/Core/ImGuiSystem.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_win32.h"
#include "ThirdParty/imgui/imgui_impl_dx11.h"

ImGuiSystem::ImGuiSystem( ImGuiConfig const& config )
{
	(void)config;
}

void ImGuiSystem::Startup()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	HWND hwnd = (HWND)g_engine->m_window->GetHwnd();
	ImGui_ImplWin32_Init( hwnd );
	ImGui_ImplDX11_Init( g_engine->m_render->GetDevice(), g_engine->m_render->GetDeviceContext() );
}

void ImGuiSystem::Shutdown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiSystem::BeginFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiSystem::EndFrame()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );
}

bool ImGuiSystem::WantCaptureMouse() const
{
	return ImGui::GetIO().WantCaptureMouse;
}
