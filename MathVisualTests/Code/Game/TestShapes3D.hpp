#pragma once

#include "Game/Game.hpp"
#include "Game/Player.hpp"
#include "Game/TestShape3D.hpp"
#include "Game/TestShapeSphere.hpp"


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
	std::vector<TestShapeSphere*> m_testShapeSpheres;
	Texture* m_modelTexture = nullptr;
	void UpdateShapesOverlap();
	bool UpdateShapesOverlapWithSphere( TestShape3D* sphereShape );
};
