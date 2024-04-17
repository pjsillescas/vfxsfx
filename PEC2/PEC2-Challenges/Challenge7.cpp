#include "Challenge7.h"

//Using SDL, SDL OpenGL, standard IO, and strings
#include <SDL.h>
#include <glad/glad.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include "src/Shader.h"
#include "src/stb/stb_image.h"
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

class Quad
{
private:
	Shader* shader;
	int lastTick;

	unsigned int VBO, VAO, EBO;

	glm::vec3 *translation;
	glm::vec3 *scale;
	float angle;
	glm::mat4 *projectionMatrix;

public:
	Quad(glm::vec3 translation, glm::vec3 scale, float initialAngle);
	~Quad();

	void update(int tick);
	void updateProjectionMatrix();
};

void Quad::updateProjectionMatrix()
{
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), *translation);
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), *scale);

	*projectionMatrix = translationMatrix * rotationMatrix * scaleMatrix;
}

Quad::Quad(glm::vec3 translation, glm::vec3 scale, float initialAngle)
{
	this->translation = new glm::vec3(1.f);
	this->scale = new glm::vec3(1.f);
	this->projectionMatrix = new glm::mat4(1.f);

	this->lastTick = 0;
	*this->translation = translation;
	*this->scale = scale;
	this->angle = initialAngle;
	*this->projectionMatrix = glm::mat4(1.0f);

	float vertices[] = {
		// positions          // colors
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f, // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f, // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	shader = new Shader("shaders/ch7.vs", "shaders/ch7.fs");
	shader->use(); // don't forget to activate/use the shader before setting uniforms!
}

Quad::~Quad()
{
	delete this->translation;
	delete this->scale;
	delete this->projectionMatrix;

	delete shader;

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Quad::update(int tick)
{
	const float SPEED = 1000.f;
	const float SPEED_FACTOR = (2.f * (float)M_PI) / SPEED;

	if (this->lastTick > 0)
	{
		float deltaAngle = (tick - lastTick) * SPEED_FACTOR;

		angle += deltaAngle;
		//std::cout << deltaAngle << " " << angle << std::endl;

		if (angle > 2.f * (float)M_PI)
		{
			angle -= 2.f * (float)M_PI;
		}
	}

	this->lastTick = tick;

	shader->use();
	updateProjectionMatrix();
	shader->setMatrix4("projectionMatrix",*projectionMatrix);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


Challenge7::Challenge7()
{
	;
}

Challenge7::~Challenge7()
{
	for (int i = 0; i < NUM_QUADS; i++)
	{
		delete quads[i];
	}
}

bool Challenge7::init()
{
	if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
	{
		printf("No OpenGL functions found %s\n", SDL_GetError());
		return false;
	}

	//Use Vsync
	if (SDL_GL_SetSwapInterval(1) < 0)
	{
		printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
	}

	return true;
}

void Challenge7::handleKeys(unsigned char key, int x, int y)
{
	;
}



void Challenge7::close()
{
	;
}

void Challenge7::render()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	int tick = SDL_GetTicks64();
	// render container
	for (int i = 0; i < NUM_QUADS; i++)
	{
		quads[i]->update(tick);
	}

}

void Challenge7::initQuads()
{
	glm::vec3 scale(0.25f, 0.25f, 0.25f);

	glm::vec3 translations[] = {
		glm::vec3(-0.5f,0.5f,0.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.5f, 0.5f, 0.f),
		glm::vec3(-0.5f,-0.5f,0.f),
		glm::vec3(0.5f, -0.5f, 0.f),
	};

	for (int i = 0; i < NUM_QUADS; i++)
	{
		float angle = 2.f * ((float)M_PI) * ((float)rand() / RAND_MAX);

		float factor = ((float)rand() / RAND_MAX) * 1.2f + 1.f;

		quads[i] = new Quad(translations[i % 5], scale * factor, angle);
	}

}

int Challenge7::run(SDL_Window* gWindow)
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
		return 0;
	}

	initQuads();

	//Main loop flag
	bool quit = false;

	//Event handler
	SDL_Event e;

	//Enable text input
	SDL_StartTextInput();
	//std::cout << "fin" << std::endl;

	//While application is running
	while (!quit)
	{
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			//Handle keypress with current mouse position
			else if (e.type == SDL_TEXTINPUT)
			{
				int x = 0, y = 0;
				SDL_GetMouseState(&x, &y);
				handleKeys(e.text.text[0], x, y);
			}
		}

		//Render quad
		render();

		//Update screen
		SDL_GL_SwapWindow(gWindow);
	}

	//Disable text input
	SDL_StopTextInput();

	//Free resources and close SDL
	close();

	return 0;
}
