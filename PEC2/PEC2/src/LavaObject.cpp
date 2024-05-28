#include "LavaObject.h"


#include "FrameBuffer.h"
#include "TextureUtils.h"
#include "Utils.h"
#include "Camera3D.h"
#include "SceneRenderer.h"
#include "FlameObject.h"

FrameBuffer* LavaObject::createFrameBuffer(int bufferWidth, int bufferHeight, int screenWidth, int screenHeight)
{
	FrameBuffer* buffer = new FrameBuffer(bufferWidth, bufferHeight, screenWidth, screenHeight);
	buffer->init();

	return buffer;
}

LavaObject::LavaObject(LavaConfig& lavaConfig, SceneRenderer* scene, FlameObject* flame)
{
	time = 0.f;
	mCurrentOffsetWave = 0.0f;
	lavaSpeed = LAVA_SPEED;

	this->camera = lavaConfig.camera;
	this->lavaShader = lavaConfig.lavaShader;
	this->scene = scene;
	this->flame = flame;

	this->loadObjFromDisk(lavaConfig.geometryFile);
	this->setShader(lavaConfig.lavaShader);
	this->setDudvTexture(TextureUtils::loadTextureFromDisk(lavaConfig.dudvTextureFile));
	this->setNormalTexture(TextureUtils::loadTextureFromDisk(lavaConfig.normalTextureFile));
	this->setLavaSpeed(lavaConfig.lavaSpeed);

	lavaReflectionFrameBuffer = createFrameBuffer(lavaConfig.reflectionWidth, lavaConfig.reflectionHeight,
		lavaConfig.screenWidth, lavaConfig.screenHeight);
}

LavaObject::~LavaObject()
{
	delete lavaReflectionFrameBuffer;
}

void LavaObject::setShader(Shader* p_shader)
{
	mShaderforDraw = p_shader;
	mUniformModelM = glGetUniformLocation(mShaderforDraw->getID(), "model");
	mUniformReflectionTex = glGetUniformLocation(mShaderforDraw->getID(), "reflectionTexture");
	mUniformDudvTex = glGetUniformLocation(mShaderforDraw->getID(), "dudvTexture");
	mUniformNormalTex = glGetUniformLocation(mShaderforDraw->getID(), "normalTexture");
	mUniformOffsetWave = glGetUniformLocation(mShaderforDraw->getID(), "flowTime");
	mUniformCamPos = glGetUniformLocation(mShaderforDraw->getID(), "camPos");
	mUniformLightColor = glGetUniformLocation(mShaderforDraw->getID(), "lightcolor");
	mUniformLightPos = glGetUniformLocation(mShaderforDraw->getID(), "lightPos");
}


void LavaObject::render()
{
	//Bind program
	lavaShader->Use();
	// Active Textures and Set them
	this->setTexture(lavaReflectionFrameBuffer->getTexture());
	
	glUniform1f(glGetUniformLocation(lavaShader->getID(), "time"), time);
	glUniform2f(glGetUniformLocation(lavaShader->getID(), "iResolution"), 7, 7);
	time += 0.02f;


	glm::mat4 projectionMatrix = camera->getUniformProjectionMatrix();
	glm::mat4 viewMatrix = camera->getUniformViewMatrix();
	Utils::fillMatrixData(lavaShader->getID(), projectionMatrix, viewMatrix);

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

void LavaObject::renderObject()
{
	//Sets texture
	if (mTexture != -1)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mTexture);
		glUniform1i(mUniformReflectionTex, 0);
	}
	if (mDudvTexture != -1)
	{
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mDudvTexture);
		glUniform1i(mUniformDudvTex, 2);
	}
	if (mNormalTexture != -1)
	{
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, mNormalTexture);
		glUniform1i(mUniformNormalTex, 3);
	}
	mCurrentOffsetWave += lavaSpeed; // It's better to control with FrameTime 
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

void LavaObject::renderFrameBuffers()
{
	// Reflection texture render
	lavaReflectionFrameBuffer->bind();
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

	lavaReflectionFrameBuffer->unbind();
}