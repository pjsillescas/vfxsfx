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
	if (mRefractionTexture != -1)
	{
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, mRefractionTexture);
		glUniform1i(mUniformRefractionTex, 5);
	}
	if (mDistortionTexture != -1)
	{
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, mDistortionTexture);
		glUniform1i(mUniformDistortionTex, 6);
	}

	mCurrentOffsetWave += WAVE_SPEED; // It's better to control with FrameTime 
	if (mCurrentOffsetWave > 1.0f)
	{
		mCurrentOffsetWave -= 1.0;
	}
	//std::cout << "currentdistord " << mUniformOffsetWave << " => " << mCurrentOffsetWave << std::endl;
	glUniform1f(mUniformOffsetWave, mCurrentOffsetWave);
	glUniform4f(mUniformBackgroundTint, 0,0,0,1);
	
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
	mUniformRefractionTex = glGetUniformLocation(mShaderforDraw->getID(), "refractionTexture");
	mUniformDistortionTex = glGetUniformLocation(mShaderforDraw->getID(), "distortionTexture");
	mUniformOffsetWave = glGetUniformLocation(mShaderforDraw->getID(), "time");
	mUniformBackgroundTint = glGetUniformLocation(mShaderforDraw->getID(), "backgroundTint");
}
