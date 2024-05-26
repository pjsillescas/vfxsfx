#pragma once

#include <SDL.h>
#include <string>
#include "EffectTemplate.h"
#include "Shader.h"

class Camera3D;
class PlayerController;
class SceneRenderer;
class FlameObject;
class LavaObject;

class Pec2Effect : public EffectTemplate
{
protected:
	// Reflection and refraction dimension constants
	const int REFLECTION_WIDTH = 320;
	const int REFLECTION_HEIGHT = 180;

	const int REFRACTION_WIDTH = 1280;
	const int REFRACTION_HEIGHT = 720;

	Shader waterShader;
	Shader textureMatrixColorShader;
	Shader textureMatrixColorShaderBackground;
	Shader flameShader;
	Shader lavaShader;

	Camera3D* camera;
	PlayerController* playerController;

	//WaterObject* waterObject;
	SceneRenderer* scene;
	FlameObject* flameObject;
	LavaObject* lavaObject;

	//OpenGL context
	SDL_GLContext gContext;
	SDL_Window* window;

public:
	Pec2Effect(SDL_Surface* surface,int screenHeight,int screenWidth, int timeout, std::string title, SDL_Window* window);
	~Pec2Effect();

	virtual void init() override;
	virtual bool update(float deltaTime) override;
	virtual void render() override;

protected:
};
