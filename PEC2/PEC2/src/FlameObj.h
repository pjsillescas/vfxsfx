#pragma once

#include "Object3D.h"

class FlameObj : public Object3D
{
private:
	const float WAVE_SPEED = 0.001f;
public:
	FlameObj();
	~FlameObj();

	void render();
	void setShader(Shader* p_shader);

	void setRefractionTexture(GLuint id) { mRefractionTexture = id; };
	GLuint getRefractionTexture() const { return mRefractionTexture; };

	void setDistortionTexture(GLuint id) { mDistortionTexture = id; };
	GLuint getDistortionTexture() const { return mDistortionTexture; };
	
	void setMaskTexture(GLuint id) { mMaskTexture = id; };
	GLuint getMaskTexture() const { return mMaskTexture; };

private:
	GLuint mRefractionTexture;
	GLuint mDistortionTexture;
	GLuint mMaskTexture;
	float mCurrentOffsetWave;
	int	mUniformRefractionTex;
	int	mUniformDistortionTex;
	int mUniformMaskTex;
	int mUniformOffsetWave;
	int mUniformBackgroundTint;
};

