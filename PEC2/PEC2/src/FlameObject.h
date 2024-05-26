#pragma once

#include "Object3D.h"
#include <glm/glm.hpp>

class SceneRenderer;
//class FlameObj;
class FrameBuffer;
class Camera3D;
class Shader;

struct FlameObjectConfig
{
	Camera3D* camera;
	Shader* flameShader;
	int refractionWidth;
	int refractionHeight;
	int screenWidth;
	int screenHeight;
	std::string geometryFile;
	glm::vec3 position;
	std::string maskTextureFile;
	std::string dudvTextureFile;
};
class FlameObject : public Object3D
{
private:
	const float SMOKE_SPEED = 0.001f;
	float smokeSpeed;

	Camera3D* camera;
	Shader* flameShader;
	SceneRenderer* scene;

	// FBO for fire
	FrameBuffer* flameFrameBuffer;

	float time;

	GLuint mRefractionTexture;
	GLuint mDistortionTexture;
	GLuint mMaskTexture;
	float mCurrentOffsetWave;
	int	mUniformRefractionTex;
	int	mUniformDistortionTex;
	int mUniformMaskTex;
	int mUniformOffsetWave;
	int mUniformBackgroundTint;
public:
	FlameObject(FlameObjectConfig& flameConfig, SceneRenderer* scene);
	~FlameObject();
	void renderFrameBuffer();
	void render();

	void bindFrameBuffer();
	void unbindFrameBuffer();

	void setSmokeSpeed(float smokeSpeed) { this->smokeSpeed = smokeSpeed; }

	void setRefractionTexture(GLuint id) { mRefractionTexture = id; };
	GLuint getRefractionTexture() const { return mRefractionTexture; };

	void setDistortionTexture(GLuint id) { mDistortionTexture = id; };
	GLuint getDistortionTexture() const { return mDistortionTexture; };

	void setMaskTexture(GLuint id) { mMaskTexture = id; };
	GLuint getMaskTexture() const { return mMaskTexture; };
private:
	FrameBuffer* createFrameBuffer(int bufferWidth, int bufferHeight, int screenWidth, int screenHeight);

	void renderObject();
	void setShader(Shader* p_shader);
};
