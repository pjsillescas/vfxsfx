#include "FireObj.h"

#include <iostream>


FireObj::FireObj()
{
	mCurrentOffsetWave = 0.0f;
}


FireObj::~FireObj()
{
}

void FireObj::render()
{
	//Sets texture
	if (mTexture != -1)
	{
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, mTexture);
		glUniform1i(mUniformTex, 4);
	}
	if (mTexture2 != -1)
	{
		//printf("print texture 5 %d\n",mTexture2);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, mTexture2);
		glUniform1i(mUniformTex2, 5);
	}
	if (mTexture3 != -1)
	{
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, mTexture3);
		glUniform1i(mUniformTex3, 6);
	}
	if (mTexture4 != -1)
	{
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, mTexture4);
		glUniform1i(mUniformTex4, 7);
	}

	mCurrentOffsetWave += WAVE_SPEED; // It's better to control with FrameTime 
	if (mCurrentOffsetWave > 1.0f)
	{
		mCurrentOffsetWave -= 1.0;
	}
	//std::cout << "currentdistord " << mUniformOffsetWave << " => " << mCurrentOffsetWave << std::endl;
	glUniform1f(mUniformOffsetWave, mCurrentOffsetWave);
	
	//Set VAO
	prepareVAO();
	//Draw VAO
	glDrawElements(GL_TRIANGLES, mIndexData.size(), GL_UNSIGNED_INT, NULL);
	//Diable VAO
	diableVAO();
}

void FireObj::setShader(Shader* p_shader)
{
	mShaderforDraw = p_shader;
	mUniformModelM = glGetUniformLocation(mShaderforDraw->getID(), "model");
	mUniformTex = glGetUniformLocation(mShaderforDraw->getID(), "texture1");
	mUniformTex2 = glGetUniformLocation(mShaderforDraw->getID(), "texture2");
	mUniformTex3 = glGetUniformLocation(mShaderforDraw->getID(), "texture3");
	mUniformTex4 = glGetUniformLocation(mShaderforDraw->getID(), "texture4");
	mUniformOffsetWave = glGetUniformLocation(mShaderforDraw->getID(), "moveDistord");
	//mUniformCamPos = glGetUniformLocation(mShaderforDraw->getID(), "camPos");
	//mUniformLightColor = glGetUniformLocation(mShaderforDraw->getID(), "lightcolor");
	//mUniformLightPos = glGetUniformLocation(mShaderforDraw->getID(), "lightPos");
}
