#include "SceneRenderer.h"

#include "Camera3D.h"
#include "Utils.h"

Object3D* SceneRenderer::createCube(CubeConfig& cubeConfig, glm::vec3 position) const
{
	Object3D* object = new Object3D();
	object->loadObjFromDisk(cubeConfig.geometryFile);
	object->loadTextureFromDisk(cubeConfig.textureFile);
	object->setShader(textureMatrixColorShader);
	object->setPosition(position);

	return object;
}

SceneRenderer::SceneRenderer(SceneRendererConfig& sceneConfig)
{
	this->textureMatrixColorShader = sceneConfig.textureMatrixColorShader;
	this->camera = sceneConfig.camera;

	backgroundObject = new BackgroundObject(sceneConfig.backgroundConfig);

	for (auto& position : sceneConfig.cubePositions)
	{
		cubes.push_back(createCube(sceneConfig.cubeConfig, position));
	}

	burningCube = createCube(sceneConfig.cubeConfig, glm::vec3(0.f, 0.f, -1.f));
	burningCube->setScale(glm::vec3(7.f, 1.f, 1.f));

	underWaterPlane = new Object3D();
	underWaterPlane->loadObjFromDisk(sceneConfig.underwaterPlaneConfig.geometryFile);
	underWaterPlane->setShader(textureMatrixColorShader);
	underWaterPlane->loadTextureFromDisk(sceneConfig.underwaterPlaneConfig.textureFile);
	underWaterPlane->setPosition(sceneConfig.underwaterPlaneConfig.position);

	//flameObject = new FlameObject(sceneConfig.flameConfig, this);
}

SceneRenderer::~SceneRenderer()
{
	delete backgroundObject;
	delete underWaterPlane;
	//delete flameObject;
	delete burningCube;
}

void SceneRenderer::render(glm::vec4 pClipPlane)
{
	//Clear color buffer & Z buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	backgroundObject->render(pClipPlane);

	//Bind program
	textureMatrixColorShader->Use();

	glm::mat4 projectionMatrix = camera->getUniformProjectionMatrix();
	glm::mat4 viewMatrix = camera->getUniformViewMatrix();
	Utils::fillMatrixData(textureMatrixColorShader->getID(), projectionMatrix, viewMatrix);

	int UniformPlaneM = glGetUniformLocation(textureMatrixColorShader->getID(), "plane");
	// Clip Plane Set
	glUniform4f(UniformPlaneM, pClipPlane.x, pClipPlane.y, pClipPlane.z, pClipPlane.w);
	underWaterPlane->render();

	for (auto& cube : cubes)
	{
		cube->render();
	}


	// Fire render logic
	burningCube->render();

	//flameObject->renderFrameBuffer();
}
