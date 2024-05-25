#pragma once

#include "Object3D.h"
#include "BackgroundObject.h"
//#include "FlameObject.h"

class Camera3D;

struct CubeConfig
{
	std::string geometryFile;
	std::string textureFile;
};

struct PlaneConfig
{
	std::string geometryFile;
	std::string textureFile;
	glm::vec3 position;
};

struct SceneRendererConfig
{
	Camera3D* camera;
	Shader* textureMatrixColorShader;
	BackgroundConfig backgroundConfig;
	CubeConfig cubeConfig;
	std::vector<glm::vec3> cubePositions;
	PlaneConfig underwaterPlaneConfig;
	glm::vec3 burningCubePosition;
	glm::vec3 burningCubeScale;
	//FlameObjectConfig flameConfig;
};

class SceneRenderer
{
private:
	Shader* textureMatrixColorShader;
	Camera3D* camera;
	//FlameObject* flameObject;

	BackgroundObject* backgroundObject;
	Object3D* burningCube;

	Object3D* underWaterPlane;
	std::vector<Object3D*> cubes;

public:
	SceneRenderer(SceneRendererConfig& sceneConfig);
	~SceneRenderer();

	void render(glm::vec4 clip);
private:
	Object3D* createCube(CubeConfig& cubeConfig, glm::vec3 position) const;
};