
struct SDL_Window;

class Quad;

class Challenge7
{
private:
	const int NUM_QUADS = 5;
	Quad* quads[5];
	
public:
	Challenge7();
	~Challenge7();

	int run(SDL_Window* gWindow);

private:
	bool init();
	void initQuads();
	void handleKeys(unsigned char key, int x, int y);
	void render();
	void close();
};
