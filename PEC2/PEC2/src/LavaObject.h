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
	FrameBuffer* lavaRefractionFrameBuffer;

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

	int time;


public:
	LavaObject(LavaConfig& lavaConfig, SceneRenderer* scene);
	~LavaObject();

	void render();
	void renderFrameBuffers();


	void setLavaSpeed(float lavaSpeed) { this->lavaSpeed = lavaSpeed; }
	float getLavaSpeed() const { return lavaSpeed; }

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

	FrameBuffer* createFrameBuffer(int bufferWidth, int bufferHeight, int screenWidth, int screenHeight);
	void renderObject();
};
