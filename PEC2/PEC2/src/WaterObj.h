#pragma once

#include "Object3D.h"

class WaterObj :
	public Object3D
{
private:
	float waveSpeed;
	const float WAVE_SPEED = 0.001f;
public:
	WaterObj();
	~WaterObj();

	void setWaveSpeed(float waveSpeed) { this->waveSpeed = waveSpeed; }
	float getWaveSpeed() const { return waveSpeed; }

	void render();
	void setShader(Shader* p_shader);

	void setTexture2(GLuint id) { mTexture2 = id; };
	GLuint getTexture2() const { return mTexture2; };

	void setTexture3(GLuint id) { mTexture3 = id; };
	GLuint getTexture3() const { return mTexture3; };

	void setTexture4(GLuint id) { mTexture4 = id; };
	GLuint getTexture4() const { return mTexture4; };

	int getUniformCamPos() const { return mUniformCamPos; };
	int getUniformLightColor() const { return mUniformLightColor; };
	int getUniformLightPos() const { return mUniformLightPos; };


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

