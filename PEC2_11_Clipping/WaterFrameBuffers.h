#pragma once
#include <glad/glad.h>

#define REFLECTION_WIDTH  320
#define REFLECTION_HEIGHT  180

#define REFRACTION_WIDTH  1280
#define REFRACTION_HEIGHT  720


class WaterFrameBuffers
{
	
public:
	WaterFrameBuffers();
	~WaterFrameBuffers();
	void cleanUp();
	void bindReflectionFrameBuffer();
	void bindRefractionFrameBuffer();
	void unbindCurrentFrameBuffer();
	int getReflectionTexture();
	int getRefractionTexture();
	int getRefractionDepthTexture();

private:
	void initialiseReflectionFrameBuffer();
	void initialiseRefractionFrameBuffer();
	void bindFrameBuffer(int frameBuffer, int width, int height);
	GLuint createFrameBuffer();
	GLuint createTextureAttachment(int width, int height);
	GLuint createDepthTextureAttachment(int width, int height);
	GLuint createDepthBufferAttachment(int width, int height);


	GLuint reflectionFrameBuffer;
	GLuint reflectionTexture;
	GLuint reflectionDepthBuffer;

	GLuint refractionFrameBuffer;
	GLuint refractionTexture;
	GLuint refractionDepthTexture;
};

