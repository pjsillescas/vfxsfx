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

private:
	GLuint mTexture2;
	int	  mUniformTex;
	int	  mUniformTex2;
};

