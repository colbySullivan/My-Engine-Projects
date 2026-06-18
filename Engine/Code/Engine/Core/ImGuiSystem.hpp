#pragma once

//------------------------------------------------------------------------------
struct ImGuiConfig
{
	bool m_isEnabled = true;
};

//------------------------------------------------------------------------------
class ImGuiSystem
{
public:
	ImGuiSystem( ImGuiConfig const& config );
	~ImGuiSystem() = default;

	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	bool WantCaptureMouse() const;
};
