#pragma once

#include "Game/Game.hpp"
#include "Game/Player.hpp"
#include "Game/TestShape3D.hpp"


//-----------------------------------------------------------------------------------------------
class TestShapes3D : public Game {

public:
	TestShapes3D( App* app );
	~TestShapes3D();

	void Startup() override;
	void Shutdown() override;
	void Update( float deltaSeconds ) override;
	void Render() const override;

	void RenderTestShapes() const;
	void SpawnInitialTestShapes();
	void UpdateSpawnNewTestShapes();
	void ClearTestShapes();


private:
	Player* m_player = nullptr;
	std::vector<TestShape3D*> m_testShapes;
	Texture* m_modelTexture = nullptr;
};
