#pragma once

#include "Object3D.h"

class FireObj : public Object3D
{
private:
	const float WAVE_SPEED = 0.001f;
public:
	FireObj();
	~FireObj();

	void render();
	void setShader(Shader* p_shader);

	void setRefractionTexture(GLuint id) { mRefractionTexture = id; };
	GLuint getRefractionTexture() const { return mRefractionTexture; };

	void setDistortionTexture(GLuint id) { mDistortionTexture = id; };
	GLuint getDistortionTexture() const { return mDistortionTexture; };

private:
	GLuint mRefractionTexture;
	GLuint mDistortionTexture;
	float mCurrentOffsetWave;
	int	mUniformRefractionTex;
	int	mUniformDistortionTex;
	int mUniformOffsetWave;
	int mUniformBackgroundTint;
};

