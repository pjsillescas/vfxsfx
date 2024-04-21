#include "WaterObj.h"

#define WAVE_SPEED 0.001f

WaterObj::WaterObj()
{
	mCurrentOffsetWave = 0.0f;
}


WaterObj::~WaterObj()
{
}

void WaterObj::render()
{
	//Sets texture
	if (mTexture != -1) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mTexture);
		glUniform1i(mUniformTex, 0);
	}
	if (mTexture2 != -1) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mTexture2);
		glUniform1i(mUniformTex2, 1);
	}
	if (mTexture3 != -1) {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mTexture3);
		glUniform1i(mUniformTex3, 2);
	}
	if (mTexture3 != -1) {
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, mTexture4);
		glUniform1i(mUniformTex4, 3);
	}
	mCurrentOffsetWave += WAVE_SPEED; // It's better to control with FrameTime 
	if (mCurrentOffsetWave > 1.0f) {
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

void WaterObj::setShader(Shader * p_shader)
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
