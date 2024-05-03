#pragma once
#include "Object3D.h"
class WaterObj :
	public Object3D
{
public:
	WaterObj();
	~WaterObj();

	void render();
	void setShader(Shader* p_shader);

	void setTexture2(GLuint id) { mTexture2 = id; };
	GLuint getTexture2() { return mTexture2; };

	void setTexture3(GLuint id) { mTexture3 = id; };
	GLuint getTexture3() { return mTexture3; };

	void setTexture4(GLuint id) { mTexture4 = id; };
	GLuint getTexture4() { return mTexture4; };

	int getUniformCamPos() { return mUniformCamPos; };
	int getUniformLightColor() { return mUniformLightColor; };
	int getUniformLightPos() { return mUniformLightPos; };


private:
	GLuint mTexture2;
	GLuint mTexture3;
	GLuint mTexture4;
	float mCurrentOffsetWave;
	int	  mUniformTex;
	int	  mUniformTex2;
	int	  mUniformTex3;
	int	  mUniformTex4;
	int   mUniformOffsetWave;
	int   mUniformCamPos;
	int	  mUniformLightColor;
	int   mUniformLightPos;
};

