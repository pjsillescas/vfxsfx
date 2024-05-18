#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(int bufferWidth, int bufferHeight, int screenWidth, int screenHeight)
{
	this->bufferWidth = bufferWidth;
	this->bufferHeight = bufferHeight;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	frameBuffer = -1;
	texture = -1;
}

FrameBuffer::~FrameBuffer()
{
	cleanUp();
}

void FrameBuffer::bind()
{
	glBindTexture(GL_TEXTURE_2D, 0);//To make sure the texture isn't bound
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glViewport(0, 0, bufferWidth, bufferHeight);
}

int FrameBuffer::getTexture() const
{
	return texture;
}

GLuint FrameBuffer::createFrameBuffer()
{
	GLuint frameBuffer;
	glGenFramebuffers(1, &frameBuffer);
	//generate name for frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	//create the framebuffer
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	//indicate that we will always render to color attachment 0
	return frameBuffer;
}

GLuint FrameBuffer::createTextureAttachment()
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bufferWidth, bufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
	return texture;
}

void FrameBuffer::init()
{
	frameBuffer = createFrameBuffer();
	texture = createTextureAttachment();
	unbind();
}

void FrameBuffer::unbind()
{
	//call to switch to default frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, screenWidth, screenHeight);
}


void FrameBuffer::cleanUp() const
{
	glDeleteFramebuffers(sizeof(frameBuffer), &frameBuffer);
	glDeleteTextures(sizeof(texture), &texture);
}
