#pragma once

#include "Game/Game.hpp"
#include "Game/Player.hpp"
#include "Game/TestShape3D.hpp"
#include "Game/TestShapeSphere.hpp"
#include "Game/TestShapeCylinder.hpp"
#include "Game/TestShapeAABB3.hpp"


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
	std::vector<TestShapeCylinder*> m_testShapeCylinder;
	std::vector<TestShapeAABB3*> m_testShapeAABB3;
	Texture* m_modelTexture = nullptr;
	void UpdateShapesOverlap();
	bool UpdateShapesOverlapWithSphere( TestShape3D* shape );
	bool UpdateShapesOverlapWithCylinder( TestShape3D* shape );
	bool UpdateShapesOverlapWithAABB3( TestShape3D* shape );
	void RaycastTestShapes() const;
	void RenderBasis() const;
	void UpdateClosePoints();
};
