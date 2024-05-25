#include "BackgroundObject.h"

#include "TextureUtils.h"
#include "Shader.h"
#include "Camera3D.h"
#include "Object3D.h"
#include "Utils.h"


BackgroundObject::BackgroundObject(BackgroundConfig& backgroundConfig)
{
	this->camera = backgroundConfig.camera;
	this->textureMatrixColorShaderBackground = backgroundConfig.textureMatrixColorShaderBackground;

	backgroundPlane = new Object3D();
	backgroundPlane->loadObjFromDisk(backgroundConfig.geometryFile);
	backgroundPlane->setTexture(TextureUtils::loadTextureFromDisk(backgroundConfig.textureFile));
	backgroundPlane->setShader(textureMatrixColorShaderBackground);
	backgroundPlane->setPosition(backgroundConfig.position);

}

BackgroundObject::~BackgroundObject()
{
	delete backgroundPlane;
}

void BackgroundObject::render(glm::vec4 clipPlane)
{
	textureMatrixColorShaderBackground->Use();

	glm::mat4 projectionMatrix = camera->getUniformProjectionMatrix();
	glm::mat4 viewMatrix = camera->getUniformViewMatrix();
	Utils::fillMatrixData(textureMatrixColorShaderBackground->getID(), projectionMatrix, viewMatrix);

	int UniformPlaneM = glGetUniformLocation(textureMatrixColorShaderBackground->getID(), "plane");
	// Clip Plane Set
	glUniform4f(UniformPlaneM, clipPlane.x, clipPlane.y, clipPlane.z, clipPlane.w);

	backgroundPlane->render();
}
