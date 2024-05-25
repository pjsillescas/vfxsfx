#include "FlameObject.h"

#include "FrameBuffer.h"
#include "FlameObj.h"
#include "TextureUtils.h"
#include "Utils.h"
#include "Camera3D.h"
#include "SceneRenderer.h"
#include <iostream>

FlameObject::FlameObject(FlameObjectConfig& flameConfig, SceneRenderer* scene)
{
	this->camera = flameConfig.camera;
	this->flameShader = flameConfig.flameShader;
	this->scene = scene;
	flamePlane = new FlameObj();
	std::cout << "mask " << flameConfig.maskTextureFile << std::endl;
	std::cout << "dudv " << flameConfig.dudvTextureFile << std::endl;
	flamePlane->loadObjFromDisk(flameConfig.geometryFile);
	flamePlane->setShader(flameShader);
	flamePlane->setPosition(flameConfig.position);
	flamePlane->setMaskTexture(TextureUtils::loadTextureFromDisk(flameConfig.maskTextureFile));
	flamePlane->setDistortionTexture(TextureUtils::loadTextureFromDisk(flameConfig.dudvTextureFile)); // Load texture and change ID to texture 3;

	// Create Frame Buffer Objects (FBO)
	flameFrameBuffer = createFrameBuffer(flameConfig.refractionWidth, flameConfig.refractionHeight, flameConfig.screenWidth, flameConfig.screenHeight);

	time = 0.f;
}

FlameObject::~FlameObject()
{
	delete flamePlane;
	delete flameFrameBuffer;
}

FrameBuffer* FlameObject::createFrameBuffer(int bufferWidth, int bufferHeight, int screenWidth, int screenHeight)
{
	FrameBuffer* buffer = new FrameBuffer(bufferWidth, bufferHeight, screenWidth, screenHeight);
	buffer->init();

	return buffer;
}

void FlameObject::render()
{
	//this->renderFrameBuffer();
	flameShader->Use();
	glUniform1f(glGetUniformLocation(flameShader->getID(), "time"), time);
	glUniform2f(glGetUniformLocation(flameShader->getID(), "iResolution"), 5, 5);
	time += 0.02f;

	glm::mat4 projectionMatrix = camera->getUniformProjectionMatrix();
	glm::mat4 viewMatrix = camera->getUniformViewMatrix();
	Utils::fillMatrixData(flameShader->getID(), projectionMatrix, viewMatrix);

	flamePlane->render();
}

void FlameObject::renderFrameBuffer()
{
	flameFrameBuffer->bind();
	flameShader->Use();
	flamePlane->setRefractionTexture(flameFrameBuffer->getTexture());

	glm::mat4 projectionMatrix = camera->getUniformProjectionMatrix();
	glm::mat4 viewMatrix = camera->getUniformViewMatrix();
	Utils::fillMatrixData(flameShader->getID(), projectionMatrix, viewMatrix);

	glm::vec4 clipPlane = glm::vec4(0, 0, -1, 1000); // 0 Height because water object ar on plane Y = 0
	scene->render(clipPlane);

	flameFrameBuffer->unbind();

	//flameShader->Use();
	//flamePlane->render();
	//this->render();
}

void FlameObject::bindFrameBuffer()
{
	flameFrameBuffer->bind();
}
void FlameObject::unbindFrameBuffer()
{
	flameFrameBuffer->unbind();
}