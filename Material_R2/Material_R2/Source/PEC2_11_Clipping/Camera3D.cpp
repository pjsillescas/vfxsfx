#include "Camera3D.h"

#include <glad/glad.h>
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"


Camera3D::Camera3D()
{
	// Same as init ("by the moment")
	mCameraSpeed = 0.05f;

	mYaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
	mPitch = 0.0f;
	mFov = 45.0f;

	mCameraFly = true;
	mGround = 0.0f;

	//Init Camera vectors
	mCameraPos = glm::vec3(0.0f, 2.0f, 3.0f);
	mCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	mCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	
	//Head
	mSensitivityTurn = 0.05f;
}



Camera3D::~Camera3D()
{
}

void Camera3D::init()
{
	mCameraSpeed = 0.05f;

	mYaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
	mPitch = 0.0f;
	mFov = 45.0f;

	mCameraFly = true;
	mGround = 0.0f;

	//Init Camera vectors
	mCameraPos = glm::vec3(0.0f, 2.0f, 6.0f);
	mCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	mCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	//Head
	mSensitivityTurn = 0.05f;
}

void Camera3D::update()
{
	glm::vec3 front;
	front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
	front.y = sin(glm::radians(mPitch));
	front.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
	mCameraFront = glm::normalize(front);
}

void Camera3D::update(bool *moveBoolArray)  // 4position up-down-left-right
{
	if (moveBoolArray[0])	//up
	{ 
		mCameraPos += mCameraSpeed * mCameraFront;
	}
	if (moveBoolArray[1])	//down
	{
		mCameraPos -= mCameraSpeed * mCameraFront;
	}
	if (moveBoolArray[2]) // left
	{
		mCameraPos -= glm::normalize(glm::cross(mCameraFront, mCameraUp)) * mCameraSpeed;
	}
	if (moveBoolArray[3])	//right
	{
		mCameraPos += glm::normalize(glm::cross(mCameraFront, mCameraUp)) * mCameraSpeed;
	}
	// No fly goes to ground
	if (!mCameraFly) {
		mCameraPos.y = mGround;
	}
}

void Camera3D::setCameraPos(float X, float Y, float Z)
{
	mCameraPos = glm::vec3(X, Y, Z);
}

void Camera3D::setCameraFront(float X, float Y, float Z)
{
	mCameraFront = glm::vec3(X, Y, Z);
}

void Camera3D::setCameraUp(float X, float Y, float Z)
{
	mCameraUp = glm::vec3(X, Y, Z);
}

void Camera3D::setUniformProjectionMatrix(int screenSizeW, int screenSizeH,int uniformID)
{
	float screenWidth = static_cast<float>(screenSizeW);
	float screenHeight = static_cast<float>(screenSizeH);
	mProjectionMatrix = glm::perspective(glm::radians(mFov), screenWidth / screenHeight, 0.1f, 100.0f);
	glUniformMatrix4fv(uniformID, 1, GL_FALSE, glm::value_ptr(mProjectionMatrix));
}

void Camera3D::setUniformViewMatrix(int uniformID)
{
	mViewMatrix = glm::lookAt(mCameraPos, mCameraPos + mCameraFront, mCameraUp);
	glUniformMatrix4fv(uniformID, 1, GL_FALSE, glm::value_ptr(mViewMatrix));
}

void Camera3D::setHeadTurn(int xOffset, int yOffset)
{
	float xoffset = static_cast<float>(xOffset);
	float yoffset = static_cast<float>(yOffset);

	xoffset *= mSensitivityTurn;
	yoffset *= mSensitivityTurn;

	mYaw += xoffset;
	mPitch += yoffset;

	if (mPitch > 89.0f)
		mPitch = 89.0f;
	if (mPitch < -89.0f)
		mPitch = -89.0f;
}

void Camera3D::setAddZoom(int value)
{
	if (mFov >= 1.0f && mFov <= 45.0f) {
		mFov -= value;
	}
	if (mFov <= 1.0f) {
		mFov = 1.0f;
	}
	if (mFov >= 45.0f) {
		mFov = 45.0f;
	}
}

glm::vec3 Camera3D::getCameraPos()
{
	return mCameraPos;
}

void Camera3D::invertPitch()
{
	mPitch = -mPitch;

	if (mPitch > 89.0f)
		mPitch = 89.0f;
	if (mPitch < -89.0f)
		mPitch = -89.0f;
}
