#pragma once

#include "Object3D.h"

class Camera3D;
class FrameBuffer;
class SceneRenderer;
class FlameObject;

struct LavaConfig
{
	Camera3D* camera;
	Shader* lavaShader;
	int reflectionWidth;
	int reflectionHeight;
	int refractionWidth;
	int refractionHeight;

	int screenWidth;
	int screenHeight;

	std::string geometryFile;
	std::string dudvTextureFile;
	std::string normalTextureFile;

	float lavaSpeed;
};

class LavaObject : public Object3D
{
private:
	const float LAVA_SPEED = 0.001f;

	float lavaSpeed;
	Camera3D* camera;
	Shader* lavaShader;
	SceneRenderer* scene;
	FlameObject* flame;

	// FBO for Lava
	FrameBuffer* lavaReflectionFrameBuffer;

	GLuint mDudvTexture;
	GLuint mNormalTexture;
	float mCurrentOffsetWave;
	int	  mUniformReflectionTex;
	int	  mUniformDudvTex;
	int	  mUniformNormalTex;
	int   mUniformOffsetWave;
	int   mUniformCamPos;
	int	  mUniformLightColor;
	int   mUniformLightPos;

	int time;

public:
	LavaObject(LavaConfig& lavaConfig, SceneRenderer* scene, FlameObject* flame);
	~LavaObject();

	void render();
	void renderFrameBuffers();

	void setLavaSpeed(float lavaSpeed) { this->lavaSpeed = lavaSpeed; }
	float getLavaSpeed() const { return lavaSpeed; }

	void setShader(Shader* p_shader);

private:
	void setDudvTexture(GLuint id) { mDudvTexture = id; };
	GLuint getDudvTexture() const { return mDudvTexture; };

	void setNormalTexture(GLuint id) { mNormalTexture = id; };
	GLuint getNormalTexture() const { return mNormalTexture; };

	int getUniformCamPos() const { return mUniformCamPos; };
	int getUniformLightColor() const { return mUniformLightColor; };
	int getUniformLightPos() const { return mUniformLightPos; };

	FrameBuffer* createFrameBuffer(int bufferWidth, int bufferHeight, int screenWidth, int screenHeight);
	void renderObject();
};
