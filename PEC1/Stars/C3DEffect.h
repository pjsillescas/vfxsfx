#pragma once

#include "EffectTemplate.h"
#include "Matrix.h"
#include "Vector.h"

class C3DEffect : public EffectTemplate
{
private:
	const int MAX_LIGHTMAP_X = 256;
	const int MAX_LIGHTMAP_Y = 256;

	// Texture
	SDL_Surface* texture;
	// buffer of 256x256 containing the light pattern (fake phong ;)
	unsigned char* light;

	// our 16 bit zbuffer
	unsigned short* zbuffer;

	// the look-up table
	unsigned char* lut;

	// properties of our torus
	const int SLICES = 32;
	const int SPANS = 16;
	const int EXT_RADIUS = 64;
	const int INT_RADIUS = 24;

// we need two structures, one that holds the position of all vertices
// in object space,  and the other in screen space. the coords in world
// space doesn't need to be stored
	struct
	{
		VECTOR* vertices, * normals;
	} org, cur;

	// this structure contains all the relevant data for each poly
	typedef struct
	{
		int p[4];  // pointer to the vertices
		int tx[4]; // static X texture index
		int ty[4]; // static Y texture index
		VECTOR normal, centre;
	} POLY;

	POLY* polies;

	// count values
	int numPolies;
	int numVertices;

	// one entry of the edge table
	typedef struct {
		int x, px, py, tx, ty, z;
	} EdgeData;

	// store two edges per horizontal line
	EdgeData **edgeTable;

	// remember the highest and the lowest point of the polygon
	int polyMinY;
	int polyMaxY;

	// object position and orientation
	MATRIX objrot;
	VECTOR objpos;

public:

	C3DEffect(SDL_Surface* surface, int screenHeight, int screenWidth);
	~C3DEffect();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;

private:
	void InitEdgeTable();
	void ScanEdge(VECTOR p1, int tx1, int ty1, int px1, int py1, VECTOR p2, int tx2, int ty2, int px2, int py2);
	void DrawSpan(int y, EdgeData* p1, EdgeData* p2);
	void DrawPolies();
	void initObject();
	void TransformPts();

};

