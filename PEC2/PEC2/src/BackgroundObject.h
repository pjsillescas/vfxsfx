#pragma once

#include <string>
#include <glm/glm.hpp>

class Shader;
class Camera3D;
class Object3D;

struct BackgroundConfig
{
	Shader* textureMatrixColorShaderBackground;
	Camera3D* camera;
	std::string geometryFile;
	std::string textureFile;
	glm::vec3 position;
};

class BackgroundObject
{
private:
	Shader* textureMatrixColorShaderBackground;
	Camera3D* camera;
	Object3D* backgroundPlane;

public:
	BackgroundObject(BackgroundConfig& backgroundConfig);
	~BackgroundObject();

	void render(glm::vec4 pClipPlane);
};
