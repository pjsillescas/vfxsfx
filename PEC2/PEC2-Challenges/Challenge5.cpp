#include "Challenge5.h"

//Using SDL, SDL OpenGL, standard IO, and strings
#include <SDL.h>
#include <glad/glad.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include "src/Shader.h"

// Triangle
class Triangle {
private:
	unsigned int VBO, VAO;
	Shader* shader;
	float offset;
public:
	Triangle(int i, float offset);
	~Triangle();

	void update(float time);

private:
	void init(int i);
};

Triangle::Triangle(int i, float offset)
{
	shader = new Shader("shaders/ch5.vs", "shaders/ch5.fs");
	shader->use();

	this->offset = offset;

	init(i);
}

Triangle::~Triangle()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	delete shader;
}

float yBottom(int i)
{
	return -1.f + 0.2f * (float)i;
}

float yTop(int i)
{
	return 1.f - 0.2f * (9.f - (float)i);
}


void Triangle::init(int i)
{
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		-0.05f, yBottom(i), 0.0f, 1.0f, // left  
		 0.05f, yBottom(i), 0.0f, 2.0f, // right 
		 0.0f,  yTop(i), 0.0f, 3.0f,  // top   
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	//glBindVertexArray(0);
	shader->use();

	shader->setVectorF3("color1", 1.0f, 0.f, 0.f);
	shader->setVectorF3("color2", 0.f, 1.f, 0.f);
	shader->setVectorF3("color3", 0.0f, 0.f, 1.f);
	shader->setFloat("offset", 0.f);
}

void Triangle::update(float time)
{
	float omega = 2.f * M_PI / 1000.f;
	float value = sinf(time * omega) / 2.f + 0.5f;

	shader->setVectorF3("color1", value, 0.f, 0.f);
	shader->setVectorF3("color2", 0.f, value, 0.f);
	shader->setVectorF3("color3", 0.0f, 0.f, value);
	shader->setFloat("offset", 0.1f * sinf(time * omega + offset));

	// draw our first triangle
	glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	glDrawArrays(GL_TRIANGLES, 0, 30);
}


bool Challenge5::init()
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


void Challenge5::handleKeys(unsigned char key, int x, int y)
{
	//Toggle quad
	if (key == 'q')
	{
		;
	}
}


void Challenge5::render()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	float time = SDL_GetTicks64();
	
	for(int i=0;i<10;i++)
	{
		triangles[i]->update(time);
	}
}

void Challenge5::close()
{
	for (int i = 0; i < NUM_TRIANGLES; i++)
	{
		delete triangles[i];
	}
}

int Challenge5::run(SDL_Window* gWindow)
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
		return 0;
	}

	for (int i = 0; i < NUM_TRIANGLES; i++)
	{
		triangles[i] = new Triangle(i, 2 * M_PI * ((float) i) / ((float)NUM_TRIANGLES));
	}

	//Main loop flag
	bool quit = false;

	//Event handler
	SDL_Event e;

	//Enable text input
	SDL_StartTextInput();

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
