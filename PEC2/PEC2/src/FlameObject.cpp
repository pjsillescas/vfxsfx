#include "FlameObject.h"

#include "FrameBuffer.h"
#include "TextureUtils.h"
#include "Utils.h"
#include "Camera3D.h"
#include "SceneRenderer.h"
#include <iostream>

FlameObject::FlameObject(FlameObjectConfig& flameConfig, SceneRenderer* scene)
{
	mCurrentOffsetWave = 0.0f;
	smokeSpeed = SMOKE_SPEED;
	this->camera = flameConfig.camera;
	this->flameShader = flameConfig.flameShader;
	this->scene = scene;
	this->loadObjFromDisk(flameConfig.geometryFile);
	this->setShader(flameShader);
	this->setPosition(flameConfig.position);
	this->setMaskTexture(TextureUtils::loadTextureFromDisk(flameConfig.maskTextureFile));
	this->setDistortionTexture(TextureUtils::loadTextureFromDisk(flameConfig.dudvTextureFile)); // Load texture and change ID to texture 3;

	// Create Frame Buffer Objects (FBO)
	flameFrameBuffer = createFrameBuffer(flameConfig.refractionWidth, flameConfig.refractionHeight, flameConfig.screenWidth, flameConfig.screenHeight);

	time = 0.f;
}

FlameObject::~FlameObject()
{
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

	this->renderObject();
}

void FlameObject::renderFrameBuffer()
{
	flameFrameBuffer->bind();
	flameShader->Use();
	this->setRefractionTexture(flameFrameBuffer->getTexture());

	glm::mat4 projectionMatrix = camera->getUniformProjectionMatrix();
	glm::mat4 viewMatrix = camera->getUniformViewMatrix();
	Utils::fillMatrixData(flameShader->getID(), projectionMatrix, viewMatrix);

	glm::vec4 clipPlane = glm::vec4(0, 0, -1, 1000); // 0 Height because water object ar on plane Y = 0
	scene->render(clipPlane);

	flameFrameBuffer->unbind();
}

void FlameObject::bindFrameBuffer()
{
	flameFrameBuffer->bind();
}
void FlameObject::unbindFrameBuffer()
{
	flameFrameBuffer->unbind();
}


void FlameObject::renderObject()
{
	//Sets texture
	if (mRefractionTexture != -1)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mRefractionTexture);
		glUniform1i(mUniformRefractionTex, 0);
	}

	if (mDistortionTexture != -1)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mDistortionTexture);
		glUniform1i(mUniformDistortionTex, 1);
	}

	if (mMaskTexture != -1)
	{
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mMaskTexture);
		glUniform1i(mUniformMaskTex, 2);
	}

	mCurrentOffsetWave += smokeSpeed; // It's better to control with FrameTime 
	if (mCurrentOffsetWave > 1.0f)
	{
		mCurrentOffsetWave -= 1.0;
	}
	//std::cout << "currentdistord " << mUniformOffsetWave << " => " << mCurrentOffsetWave << std::endl;
	glUniform1f(mUniformOffsetWave, mCurrentOffsetWave);
	glUniform4f(mUniformBackgroundTint, 0, 0, 0, 1);

	//Set VAO
	prepareVAO();
	//Draw VAO
	glDrawElements(GL_TRIANGLES, mIndexData.size(), GL_UNSIGNED_INT, NULL);
	//Diable VAO
	diableVAO();
}

void FlameObject::setShader(Shader* p_shader)
{
	mShaderforDraw = p_shader;
	mUniformRefractionTex = glGetUniformLocation(mShaderforDraw->getID(), "refractionTexture");
	mUniformDistortionTex = glGetUniformLocation(mShaderforDraw->getID(), "distortionTexture");
	mUniformMaskTex = glGetUniformLocation(mShaderforDraw->getID(), "maskTexture");
	mUniformOffsetWave = glGetUniformLocation(mShaderforDraw->getID(), "flowTime");
	mUniformBackgroundTint = glGetUniformLocation(mShaderforDraw->getID(), "backgroundTint");
}
