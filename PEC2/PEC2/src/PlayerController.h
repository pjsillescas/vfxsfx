#pragma once

#include <SDL.h>

class Camera3D;

class PlayerController
{
public:
	PlayerController(Camera3D* camera);
	
	bool handleInput();
	void update();

private:
	Camera3D* camera;
	enum direction { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT, _DIR_LAST_ };
	bool		movement[_DIR_LAST_];

	void init();
	bool handleKeys(SDL_Event& e);
	

};

