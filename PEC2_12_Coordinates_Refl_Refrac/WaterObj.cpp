#include "WaterObj.h"



WaterObj::WaterObj()
{
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
}
