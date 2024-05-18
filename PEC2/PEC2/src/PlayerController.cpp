#include "PlayerController.h"

#include "Camera3D.h"

PlayerController::PlayerController(Camera3D* camera)
{
	this->camera = camera;
	init();
}

void PlayerController::init()
{
	// Init Movements
	for (int i = 0; i < _DIR_LAST_; i++)
	{
		movement[i] = false;
	}
}

void PlayerController::update()
{
	// Move Camera
	camera->update(movement);

}

bool PlayerController::handleKeys(SDL_Event& e)
{
	bool quit = false;

	if (e.type == SDL_KEYDOWN)
	{
		if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
		{
			quit = true;
		}

		// Camera Move setting
		if ((e.key.keysym.scancode == SDL_SCANCODE_UP) ||
			(e.key.keysym.scancode == SDL_SCANCODE_W))
		{
			movement[DIR_UP] = true;
		}
		if ((e.key.keysym.scancode == SDL_SCANCODE_DOWN) ||
			(e.key.keysym.scancode == SDL_SCANCODE_S))
		{
			movement[DIR_DOWN] = true;
		}
		if ((e.key.keysym.scancode == SDL_SCANCODE_LEFT) ||
			(e.key.keysym.scancode == SDL_SCANCODE_A))
		{
			movement[DIR_LEFT] = true;
		}
		if ((e.key.keysym.scancode == SDL_SCANCODE_RIGHT) ||
			(e.key.keysym.scancode == SDL_SCANCODE_D))
		{
			movement[DIR_RIGHT] = true;
		}
	}

	if (e.type == SDL_KEYUP)
	{
		// Camera Move Stop
		if ((e.key.keysym.scancode == SDL_SCANCODE_UP) ||
			(e.key.keysym.scancode == SDL_SCANCODE_W))
		{
			movement[DIR_UP] = false;
		}
		if ((e.key.keysym.scancode == SDL_SCANCODE_DOWN) ||
			(e.key.keysym.scancode == SDL_SCANCODE_S))
		{
			movement[DIR_DOWN] = false;
		}
		if ((e.key.keysym.scancode == SDL_SCANCODE_LEFT) ||
			(e.key.keysym.scancode == SDL_SCANCODE_A))
		{
			movement[DIR_LEFT] = false;
		}
		if ((e.key.keysym.scancode == SDL_SCANCODE_RIGHT) ||
			(e.key.keysym.scancode == SDL_SCANCODE_D))
		{
			movement[DIR_RIGHT] = false;
		}
	}

	// Capture Mouse
	if (e.type == SDL_MOUSEMOTION)
	{
		camera->setHeadTurn(e.motion.xrel, -e.motion.yrel);
	}

	// Capture Mouse wheel for Zoom
	if (e.type == SDL_MOUSEWHEEL)
	{
		camera->setAddZoom(e.wheel.y);
	}

	//User requests quit
	if (e.type == SDL_QUIT)
	{
		quit = true;
	}

	return quit;
}

bool PlayerController::handleInput()
{
	bool quit = false;

	//Event handler
	SDL_Event e;
	//Handle events on queue
	while (SDL_PollEvent(&e) != 0)
	{
		quit |= handleKeys(e);
	}

	return quit;
}