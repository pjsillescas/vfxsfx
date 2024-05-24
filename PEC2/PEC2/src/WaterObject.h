#pragma once

#include "Object3D.h"

class Camera3D;
class WaterObj;
class FrameBuffer;

struct WaterConfig
{
	Camera3D* camera;
	Shader* waterShader;
	int reflectionWidth;
	int reflectionHeight;
	int refractionWidth;
	int refractionHeight;

	int screenWidth;
	int screenHeight;

	std::string geometryFile;
	std::string dudvTextureFile;
	std::string normalTextureFile;
};

class WaterObject
{
private:
	Camera3D* camera;
	Shader* waterShader;
	WaterObj* waterPlane;

	// FBO for Water
	FrameBuffer* waterReflectionFrameBuffer;
	FrameBuffer* waterRefractionFrameBuffer;

public:
	WaterObject(WaterConfig& waterConfig);
	~WaterObject();

	void render();
	void renderFrameBuffers();

private:
	FrameBuffer* createFrameBuffer(int bufferWidth, int bufferHeight, int screenWidth, int screenHeight);
};
