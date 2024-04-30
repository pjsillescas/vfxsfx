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

	int getUniformCamPos() { return mUniformCamPos; };


private:
	GLuint mTexture2;
	GLuint mTexture3;
	float mCurrentOffsetWave;
	int	  mUniformTex;
	int	  mUniformTex2;
	int	  mUniformTex3;
	int   mUniformOffsetWave;
	int   mUniformCamPos;
};

