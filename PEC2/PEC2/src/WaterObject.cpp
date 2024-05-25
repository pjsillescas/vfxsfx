#include "WaterObject.h"

#include "FrameBuffer.h"
#include "WaterObj.h"
#include "TextureUtils.h"
#include "Utils.h"
#include "Camera3D.h"
#include "SceneRenderer.h"
#include "FlameObject.h"

FrameBuffer* WaterObject::createFrameBuffer(int bufferWidth, int bufferHeight, int screenWidth, int screenHeight)
{
	FrameBuffer* buffer = new FrameBuffer(bufferWidth, bufferHeight, screenWidth, screenHeight);
	buffer->init();

	return buffer;
}

WaterObject::WaterObject(WaterConfig& waterConfig, SceneRenderer* scene, FlameObject* flame)
{
	this->camera = waterConfig.camera;
	this->waterShader = waterConfig.waterShader;
	this->scene = scene;
	this->flame = flame;
	waterPlane = new WaterObj();
	waterPlane->loadObjFromDisk(waterConfig.geometryFile);
	waterPlane->setShader(waterConfig.waterShader);
	waterPlane->setTexture3(TextureUtils::loadTextureFromDisk(waterConfig.dudvTextureFile));
	waterPlane->setTexture4(TextureUtils::loadTextureFromDisk(waterConfig.normalTextureFile));
	waterPlane->setWaveSpeed(waterConfig.waterSpeed);

	waterReflectionFrameBuffer = createFrameBuffer(waterConfig.reflectionWidth, waterConfig.reflectionHeight,
		waterConfig.screenWidth, waterConfig.screenHeight);
	waterRefractionFrameBuffer = createFrameBuffer(waterConfig.refractionWidth, waterConfig.refractionHeight,
		waterConfig.screenWidth, waterConfig.screenHeight);
}

WaterObject::~WaterObject()
{
	delete waterPlane;
	delete waterReflectionFrameBuffer;
	delete waterRefractionFrameBuffer;
}

void WaterObject::render()
{
	//Bind program
	waterShader->Use();
	// Active Textures and Set them
	waterPlane->setTexture(waterReflectionFrameBuffer->getTexture());
	waterPlane->setTexture2(waterRefractionFrameBuffer->getTexture());

	glm::mat4 projectionMatrix = camera->getUniformProjectionMatrix();
	glm::mat4 viewMatrix = camera->getUniformViewMatrix();
	Utils::fillMatrixData(waterShader->getID(), projectionMatrix, viewMatrix);

	// Give Camera Vector to Shader
	glm::vec3 camPos = camera->getCameraPos();
	glUniform3f(waterPlane->getUniformCamPos(), camPos.x, camPos.y, camPos.z);
	// Give an imaginary Sun light position and color
	glm::vec3 lightPos = glm::vec3(-3.0, 10.0, -5.0);
	glm::vec3 lightColor = glm::vec3(1.0, 1.0, 1.0);
	glUniform3f(waterPlane->getUniformLightPos(), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(waterPlane->getUniformLightColor(), lightColor.x, lightColor.y, lightColor.z);

	// Draw objects
	waterPlane->render();
}

void WaterObject::renderFrameBuffers()
{
	// Reflection texture render
	waterReflectionFrameBuffer->bind();
	// Recalculate Camera position for Render Reflection
	float distance = 2 * (camera->getCameraPos().y - waterPlane->getPosition().y);
	camera->setCameraPos(camera->getCameraPos().x, camera->getCameraPos().y - distance, camera->getCameraPos().z);
	camera->invertPitch();
	camera->update();
	glm::vec4 clipPlane = glm::vec4(0, 1, 0, 0); // 0 Height because water object ar on plane Y = 0
	scene->render(clipPlane);
	
	flame->render();

	// Camera Recovery position
	camera->setCameraPos(camera->getCameraPos().x, camera->getCameraPos().y + distance, camera->getCameraPos().z);
	camera->invertPitch();
	camera->update();

	// Refraction texture render
	waterRefractionFrameBuffer->bind();
	
	flame->render();

	clipPlane = glm::vec4(0, -1, 0, 0); // 0 Height because water object ar on plane Y = 0
	scene->render(clipPlane);

	waterRefractionFrameBuffer->unbind();
}