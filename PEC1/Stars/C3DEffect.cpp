#include "C3DEffect.h"

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <cmath>

#include "Clock.h"

C3DEffect::C3DEffect(SDL_Surface* surface, int screenHeight, int screenWidth) : EffectTemplate(surface, screenHeight, screenWidth)
{
	edgeTable = new EdgeData*[screenHeight];
	for (int i = 0; i < screenHeight; i++)
	{
		edgeTable[i] = new EdgeData[2];
	}

	// prepare the lighting
	light = new unsigned char[MAX_LIGHTMAP_X * MAX_LIGHTMAP_Y];
	// prepare 3D data
	zbuffer = new unsigned short[screenWidth * screenHeight];
}

bool C3DEffect::isEnded()
{
	return false;
}

void C3DEffect::init()
{
	// Load Texture
	SDL_Surface* temp = IMG_Load("texture_torus.png");
	if (temp == NULL) {
		std::cout << "Image can be loaded! " << IMG_GetError();
		exit(1);
	}
	texture = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_ARGB8888, 0);

	for (int j = 0; j < MAX_LIGHTMAP_X; j++)
	{
		for (int i = 0; i < MAX_LIGHTMAP_Y; i++)
		{
			// calculate distance from the centre
			int c = ((128 - i) * (128 - i) + (128 - j) * (128 - j)) / 35;
			// check for overflow
			if (c > 255) c = 255;
			// store lumel
			light[(j << 8) + i] = 255 - c;
		}
	}
	
	initObject();

}

void C3DEffect::update(float deltaTime)
{
	int currentTime = Clock::getInstance().getCurrentTime();

	// clear the zbuffer
	memset(zbuffer, 255, screenWidth * screenHeight * sizeof(unsigned short));
	// set the torus' rotation
	objrot = rotX((float)currentTime / 1100 + M_PI * cos((float)currentTime / 2200))
		* rotY((float)currentTime / 1300 + M_PI * sin((float)currentTime / 2600))
		* rotZ((float)currentTime / 1500 - M_PI * cos((float)currentTime / 2500));
	// and it's position
	objpos = VECTOR(
		48 * cos((float)currentTime / 1266.0f),
		48 * sin((float)currentTime / 1424.0f),
		200 + 80 * sin((float)currentTime / 1912.0f));
	// rotate and project our points
	TransformPts();
}

void C3DEffect::render()
{
	// clear the background
	SDL_FillRect(surface, NULL, 0);
	// and draw the polygons
	DrawPolies();
}

C3DEffect::~C3DEffect()
{
	for (int i = 0; i < screenHeight; i++)
	{
		delete[] edgeTable;
	}

	delete[] edgeTable;

	SDL_FreeSurface(texture);
	delete[] zbuffer;
	delete[] org.vertices;
	delete[] org.normals;
	delete[] cur.vertices;
	delete[] cur.normals;
	delete[] polies;

}

/*
* clears all entries in the edge table
*/
void C3DEffect::InitEdgeTable()
{
	for (int i = 0; i < screenHeight; i++)
	{
		edgeTable[i][0].x = -1;
		edgeTable[i][1].x = -1;
	}
	polyMinY = screenHeight;
	polyMaxY = -1;
}

/*
* scan along one edge of the poly, i.e. interpolate all values and store
* in the edge table
*/
void C3DEffect::ScanEdge(VECTOR p1, int tx1, int ty1, int px1, int py1,
	VECTOR p2, int tx2, int ty2, int px2, int py2)
{
	// we can't handle this case, so we recall the proc with reversed params
	// saves having to swap all the vars, but it's not good practice
	if (p2[1] < p1[1]) {
		ScanEdge(p2, tx2, ty2, px2, py2, p1, tx1, ty1, px1, py1);
		return;
	}
	// convert to fixed point
	int x1 = (int)(p1[0] * 65536),
		y1 = (int)(p1[1]),
		z1 = (int)(p1[2] * 16),
		x2 = (int)(p2[0] * 65536),
		y2 = (int)(p2[1]),
		z2 = (int)(p2[2] * 16);
	// update the min and max of the current polygon
	if (y1 < polyMinY) polyMinY = y1;
	if (y2 > polyMaxY) polyMaxY = y2;
	// compute deltas for interpolation
	int dy = y2 - y1;
	if (dy == 0) return;
	int dx = (x2 - x1) / dy,                // assume 16.16 fixed point
		dtx = (tx2 - tx1) / dy,
		dty = (ty2 - ty1) / dy,
		dpx = (px2 - px1) / dy,
		dpy = (py2 - py1) / dy,
		dz = (z2 - z1) / dy;              // probably 12.4, but doesn't matter
										  // interpolate along the edge
	for (int y = y1; y < y2; y++)
	{
		// don't go out of the screen
		if (y > (screenHeight - 1)) return;
		// only store if inside the screen, we should really clip
		if (y >= 0)
		{
			// is first slot free?
			if (edgeTable[y][0].x == -1)
			{ // if so, use that
				edgeTable[y][0].x = x1;
				edgeTable[y][0].tx = tx1;
				edgeTable[y][0].ty = ty1;
				edgeTable[y][0].px = px1;
				edgeTable[y][0].py = py1;
				edgeTable[y][0].z = z1;
			}
			else { // otherwise use the other
				edgeTable[y][1].x = x1;
				edgeTable[y][1].tx = tx1;
				edgeTable[y][1].ty = ty1;
				edgeTable[y][1].px = px1;
				edgeTable[y][1].py = py1;
				edgeTable[y][1].z = z1;
			}
		}

		// interpolate our values
		x1 += dx;
		px1 += dpx;
		py1 += dpy;
		tx1 += dtx;
		ty1 += dty;
		z1 += dz;
	}
}

/*
* draw a horizontal double textured span
*/
void C3DEffect::DrawSpan(int y, EdgeData* p1, EdgeData* p2)
{
	// quick check, if facing back then draw span in the other direction,
	// avoids having to swap all the vars... not a very elegant
	if (p1->x > p2->x)
	{
		DrawSpan(y, p2, p1);
		return;
	};
	// load starting points
	int z1 = p1->z,
		px1 = p1->px,
		py1 = p1->py,
		tx1 = p1->tx,
		ty1 = p1->ty,
		x1 = p1->x >> 16,
		x2 = p2->x >> 16;
	// check if it's inside the screen
	if ((x1 > (screenWidth - 1)) || (x2 < 0)) return;
	// compute deltas for interpolation
	int dx = x2 - x1;
	if (dx == 0) return;
	int dtx = (p2->tx - p1->tx) / dx,  // assume 16.16 fixed point
		dty = (p2->ty - p1->ty) / dx,
		dpx = (p2->px - p1->px) / dx,
		dpy = (p2->py - p1->py) / dx,
		dz = (p2->z - p1->z) / dx;

	// setup the offsets in the buffers
	Uint8* dst;
	Uint8* initbuffer = (Uint8*)surface->pixels;
	int bpp = surface->format->BytesPerPixel;
	Uint8* imagebuffer = (Uint8*)texture->pixels;
	int bppImage = texture->format->BytesPerPixel;

	// get destination offset in buffer
	long offs = y * screenWidth + x1;
	// loop for all pixels concerned
	for (int i = x1; i < x2; i++)
	{
		if (i > (screenWidth - 1)) return;
		// check z buffer
		if (i >= 0) if (z1 < zbuffer[offs])
		{
			// if visible load the texel from the translated texture
			Uint8* p = (Uint8*)imagebuffer + ((ty1 >> 16) & 0xff) * texture->pitch + ((tx1 >> 16) & 0xFF) * bppImage;
			SDL_Color ColorTexture;
			SDL_GetRGB(*(Uint32*)(p), texture->format, &ColorTexture.r, &ColorTexture.g, &ColorTexture.b);
			// and the texel from the light map
			unsigned char LightFactor = light[((py1 >> 8) & 0xff00) + ((px1 >> 16) & 0xff)];
			// mix them together, and store
			int ColorR = (ColorTexture.r + LightFactor);
			if (ColorR > 255)
				ColorR = 255;
			int ColorG = (ColorTexture.g + LightFactor);
			if (ColorG > 255)
				ColorG = 255;
			int ColorB = (ColorTexture.b + LightFactor);
			if (ColorB > 255)
				ColorB = 255;
			Uint32 resultColor = 0xFF000000 | (ColorR << 16) | (ColorG << 8) | ColorB;
			dst = initbuffer + y * surface->pitch + i * bpp;
			*(Uint32*)dst = resultColor;
			// and update the zbuffer
			zbuffer[offs] = z1;
		}
		// interpolate our values
		px1 += dpx;
		py1 += dpy;
		tx1 += dtx;
		ty1 += dty;
		z1 += dz;
		// and find next pixel
		offs++;
	}
}

/*
* cull and draw the visible polies
*/
void C3DEffect::DrawPolies()
{
	int i;
	for (int n = 0; n < numPolies; n++)
	{
		// rotate the centre and normal of the poly to check if it is actually visible.
		VECTOR ncent = objrot * polies[n].centre,
			nnorm = objrot * polies[n].normal;

		// calculate the dot product, and check it's sign
		if ((ncent[0] + objpos[0]) * nnorm[0]
			+ (ncent[1] + objpos[1]) * nnorm[1]
			+ (ncent[2] + objpos[2]) * nnorm[2] < 0)
		{
			// the polygon is visible, so setup the edge table
			InitEdgeTable();
			// process all our edges
			for (i = 0; i < 4; i++)
			{
				ScanEdge(
					// the vertex in screen space
					cur.vertices[polies[n].p[i]],
					// the static texture coordinates
					polies[n].tx[i], polies[n].ty[i],
					// the dynamic text coords computed with the normals
					(int)(65536 * (128 + 127 * cur.normals[polies[n].p[i]][0])),
					(int)(65536 * (128 + 127 * cur.normals[polies[n].p[i]][1])),
					// second vertex in screen space
					cur.vertices[polies[n].p[(i + 1) & 3]],
					// static text coords
					polies[n].tx[(i + 1) & 3], polies[n].ty[(i + 1) & 3],
					// dynamic texture coords
					(int)(65536 * (128 + 127 * cur.normals[polies[n].p[(i + 1) & 3]][0])),
					(int)(65536 * (128 + 127 * cur.normals[polies[n].p[(i + 1) & 3]][1]))
				);
			}
			// quick clipping
			if (polyMinY < 0) polyMinY = 0;
			if (polyMaxY > screenHeight) polyMaxY = screenHeight;
			// do we have to draw anything?
			if ((polyMinY < polyMaxY) && (polyMaxY > 0) && (polyMinY < screenHeight))
			{
				// if so just draw relevant lines
				for (i = polyMinY; i < polyMaxY; i++)
				{
					DrawSpan(i, &edgeTable[i][0], &edgeTable[i][1]);
				}
			}
		}
	}
}

/*
* generate a torus object
*/
void C3DEffect::initObject()
{
	// allocate necessary memory for points and their normals
	numVertices = SLICES * SPANS;
	org.vertices = new VECTOR[numVertices];
	cur.vertices = new VECTOR[numVertices];
	org.normals = new VECTOR[numVertices];
	cur.normals = new VECTOR[numVertices];
	int i, j, k = 0;
	// now create all the points and their normals, start looping
	// round the origin (circle C1)
	for (i = 0; i < SLICES; i++)
	{
		// find angular position
		float ext_angle = (float)i * (float) M_PI * 2.0f / SLICES,
			ca = cos(ext_angle),
			sa = sin(ext_angle);
		// now loop round C2
		for (j = 0; j < SPANS; j++)
		{
			float int_angle = (float)j * (float) M_PI * 2.0f / SPANS,
				int_rad = EXT_RADIUS + INT_RADIUS * cos(int_angle);
			// compute position of vertex by rotating it round C1
			org.vertices[k] = VECTOR(
				int_rad * ca,
				INT_RADIUS * sin(int_angle),
				int_rad * sa);
			// then find the normal, i.e. the normalised vector representing the
			// distance to the correpsonding point on C1
			org.normals[k] = normalize(org.vertices[k] -
				VECTOR(EXT_RADIUS * ca, 0, EXT_RADIUS * sa));
			k++;
		}
	}

	// now initialize the polygons, there are as many quads as vertices
	numPolies = SPANS * SLICES;
	polies = new POLY[numPolies];
	// perform the same loop
	for (i = 0; i < SLICES; i++)
	{
		for (j = 0; j < SPANS; j++)
		{
			POLY& P = polies[i * SPANS + j];

			// setup the pointers to the 4 concerned vertices
			P.p[0] = i * SPANS + j;
			P.p[1] = i * SPANS + ((j + 1) % SPANS);
			P.p[3] = ((i + 1) % SLICES) * SPANS + j;
			P.p[2] = ((i + 1) % SLICES) * SPANS + ((j + 1) % SPANS);

			// now compute the static texture refs (X)
			P.tx[0] = (i * 512 / SLICES) << 16;
			P.tx[1] = (i * 512 / SLICES) << 16;
			P.tx[3] = ((i + 1) * 512 / SLICES) << 16;
			P.tx[2] = ((i + 1) * 512 / SLICES) << 16;

			// now compute the static texture refs (Y)
			P.ty[0] = (j * 512 / SPANS) << 16;
			P.ty[1] = ((j + 1) * 512 / SPANS) << 16;
			P.ty[3] = (j * 512 / SPANS) << 16;
			P.ty[2] = ((j + 1) * 512 / SPANS) << 16;

			// get the normalized diagonals
			VECTOR d1 = normalize(org.vertices[P.p[2]] - org.vertices[P.p[0]]),
				d2 = normalize(org.vertices[P.p[3]] - org.vertices[P.p[1]]),
				// and their dot product
				temp = VECTOR(d1[1] * d2[2] - d1[2] * d2[1],
					d1[2] * d2[0] - d1[0] * d2[2],
					d1[0] * d2[1] - d1[1] * d2[0]);
			// normalize that and we get the face's normal
			P.normal = normalize(temp);

			// the centre of the face is just the average of the 4 corners
			// we could use this for depth sorting
			temp = org.vertices[P.p[0]] + org.vertices[P.p[1]]
				+ org.vertices[P.p[2]] + org.vertices[P.p[3]];
			P.centre = VECTOR(temp[0] * 0.25f, temp[1] * 0.25f, temp[2] * 0.25f);
		}
	}
}

/*
* rotate and project all vertices, and just rotate point normals
*/
void C3DEffect::TransformPts()
{
	for (int i = 0; i < numVertices; i++)
	{
		// perform rotation
		cur.normals[i] = objrot * org.normals[i];
		cur.vertices[i] = objrot * org.vertices[i];
		// now project onto the screen
		cur.vertices[i][2] += objpos[2];
		cur.vertices[i][0] = screenHeight * (cur.vertices[i][0] + objpos[0]) / cur.vertices[i][2] + (screenWidth / 2);
		cur.vertices[i][1] = screenHeight * (cur.vertices[i][1] + objpos[1]) / cur.vertices[i][2] + (screenHeight / 2);
	}
}
