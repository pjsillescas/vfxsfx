#include "FlameObj.h"

#include <iostream>


FlameObj::FlameObj()
{
	mCurrentOffsetWave = 0.0f;
}


FlameObj::~FlameObj()
{
}

void FlameObj::render()
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

void FlameObj::setShader(Shader* p_shader)
{
	mShaderforDraw = p_shader;
	mUniformRefractionTex = glGetUniformLocation(mShaderforDraw->getID(), "refractionTexture");
	mUniformDistortionTex = glGetUniformLocation(mShaderforDraw->getID(), "distortionTexture");
	mUniformMaskTex = glGetUniformLocation(mShaderforDraw->getID(), "maskTexture");
	mUniformOffsetWave = glGetUniformLocation(mShaderforDraw->getID(), "flowTime");
	mUniformBackgroundTint = glGetUniformLocation(mShaderforDraw->getID(), "backgroundTint");
}
