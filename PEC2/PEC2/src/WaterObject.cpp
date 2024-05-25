#include "WaterObject.h"

#include "FrameBuffer.h"
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
	mCurrentOffsetWave = 0.0f;
	waveSpeed = WAVE_SPEED;

	this->camera = waterConfig.camera;
	this->waterShader = waterConfig.waterShader;
	this->scene = scene;
	this->flame = flame;

	this->loadObjFromDisk(waterConfig.geometryFile);
	this->setShader(waterConfig.waterShader);
	this->setTexture3(TextureUtils::loadTextureFromDisk(waterConfig.dudvTextureFile));
	this->setTexture4(TextureUtils::loadTextureFromDisk(waterConfig.normalTextureFile));
	this->setWaveSpeed(waterConfig.waterSpeed);

	waterReflectionFrameBuffer = createFrameBuffer(waterConfig.reflectionWidth, waterConfig.reflectionHeight,
		waterConfig.screenWidth, waterConfig.screenHeight);
	waterRefractionFrameBuffer = createFrameBuffer(waterConfig.refractionWidth, waterConfig.refractionHeight,
		waterConfig.screenWidth, waterConfig.screenHeight);
}

WaterObject::~WaterObject()
{
	//delete waterPlane;
	delete waterReflectionFrameBuffer;
	delete waterRefractionFrameBuffer;
}

void WaterObject::setShader(Shader* p_shader)
{
	mShaderforDraw = p_shader;
	mUniformModelM = glGetUniformLocation(mShaderforDraw->getID(), "model");
	mUniformTex = glGetUniformLocation(mShaderforDraw->getID(), "texture1");
	mUniformTex2 = glGetUniformLocation(mShaderforDraw->getID(), "texture2");
	mUniformTex3 = glGetUniformLocation(mShaderforDraw->getID(), "texture3");
	mUniformTex4 = glGetUniformLocation(mShaderforDraw->getID(), "texture4");
	mUniformOffsetWave = glGetUniformLocation(mShaderforDraw->getID(), "moveDistord");
	mUniformCamPos = glGetUniformLocation(mShaderforDraw->getID(), "camPos");
	mUniformLightColor = glGetUniformLocation(mShaderforDraw->getID(), "lightcolor");
	mUniformLightPos = glGetUniformLocation(mShaderforDraw->getID(), "lightPos");
}


void WaterObject::render()
{
	//Bind program
	waterShader->Use();
	// Active Textures and Set them
	this->setTexture(waterReflectionFrameBuffer->getTexture());
	this->setTexture2(waterRefractionFrameBuffer->getTexture());

	glm::mat4 projectionMatrix = camera->getUniformProjectionMatrix();
	glm::mat4 viewMatrix = camera->getUniformViewMatrix();
	Utils::fillMatrixData(waterShader->getID(), projectionMatrix, viewMatrix);

	// Give Camera Vector to Shader
	glm::vec3 camPos = camera->getCameraPos();
	glUniform3f(this->getUniformCamPos(), camPos.x, camPos.y, camPos.z);
	// Give an imaginary Sun light position and color
	glm::vec3 lightPos = glm::vec3(-3.0, 10.0, -5.0);
	glm::vec3 lightColor = glm::vec3(1.0, 1.0, 1.0);
	
	glUniform3f(this->getUniformLightPos(), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(this->getUniformLightColor(), lightColor.x, lightColor.y, lightColor.z);

	// Draw objects
	this->renderObject();
}

void WaterObject::renderObject()
{
	//Sets texture
	if (mTexture != -1)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mTexture);
		glUniform1i(mUniformTex, 0);
	}
	if (mTexture2 != -1)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mTexture2);
		glUniform1i(mUniformTex2, 1);
	}
	if (mTexture3 != -1)
	{
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mTexture3);
		glUniform1i(mUniformTex3, 2);
	}
	if (mTexture4 != -1)
	{
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, mTexture4);
		glUniform1i(mUniformTex4, 3);
	}
	mCurrentOffsetWave += waveSpeed; // It's better to control with FrameTime 
	if (mCurrentOffsetWave > 1.0f)
	{
		mCurrentOffsetWave -= 1.0;
	}
	glUniform1f(mUniformOffsetWave, mCurrentOffsetWave);
	//Set VAO
	prepareVAO();
	//Draw VAO
	glDrawElements(GL_TRIANGLES, mIndexData.size(), GL_UNSIGNED_INT, NULL);
	//Diable VAO
	diableVAO();
}

void WaterObject::renderFrameBuffers()
{
	// Reflection texture render
	waterReflectionFrameBuffer->bind();
	// Recalculate Camera position for Render Reflection
	float distance = 2 * (camera->getCameraPos().y - this->getPosition().y);
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