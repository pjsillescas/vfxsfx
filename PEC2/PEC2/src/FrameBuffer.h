#pragma once

#include <glad/glad.h>

class FrameBuffer {
private:
	int bufferWidth;
	int bufferHeight;
	int screenWidth;
	int screenHeight;

	GLuint frameBuffer;
	GLuint texture;

public:
	FrameBuffer(int bufferWidth, int bufferHeight, int screenWidth, int screenHeight);
	~FrameBuffer();

	void bind();
	void unbind();
	int getTexture() const;
	void init();
	void cleanUp() const;

private:
	GLuint createFrameBuffer();
	GLuint createTextureAttachment();
};
