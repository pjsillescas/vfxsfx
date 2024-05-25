#pragma once

#include "Object3D.h"
#include <glm/glm.hpp>

class SceneRenderer;
class FlameObj;
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
class FlameObject
{
private:
	Camera3D* camera;
	Shader* flameShader;
	FlameObj* flamePlane;
	SceneRenderer* scene;

	// FBO for fire
	FrameBuffer* flameFrameBuffer;

	float time;
public:
	FlameObject(FlameObjectConfig& flameConfig, SceneRenderer* scene);
	~FlameObject();
	void renderFrameBuffer();
	void render();

	void bindFrameBuffer();
	void unbindFrameBuffer();
private:
	FrameBuffer* createFrameBuffer(int bufferWidth, int bufferHeight, int screenWidth, int screenHeight);
};
