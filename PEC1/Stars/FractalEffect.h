#pragma once

#include "EffectTemplate.h"

class FractalEffect : public EffectTemplate
{
private:
	const float ZOOM_IN_FACTOR = 0.5f;
	const float ZOOM_OUT_FACTOR = 2.f;
	
	const double OR = -0.577816 - 9.31323E-10 - 1.16415E-10;
	const double OI = -0.631121 - 2.38419E-07 + 1.49012E-08;
	
	// our precalculated mandelbrot fractals
	unsigned char* frac1;
	unsigned char* frac2;
	unsigned char* fractmp;

	// define the point in the complex plane to which we will zoom into
	// set original zooming settings
	//    double zx = 2.91038E-11, zy = 2.91038E-11;
	//    bool zoom_in = false;
	double zx = 4.0;
	double zy = 4.0;
	bool zoom_in = true;

	/*
	 * global variables used for calculating our fractal
	 */
	double dr;
	double di;
	double pr;
	double pi;
	double sr;
	double si;
	long offs;
	
	// setup the palette
	int i;
	int j = 0;
	int k = 0;
	
	struct RGBColor { unsigned char R, G, B; };
	RGBColor palette[256];

public:

	FractalEffect(SDL_Surface* surface, int screenHeight, int screenWidth);
	~FractalEffect();

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void render() override;

private:
	void StartFrac(double _sr, double _si, double er, double ei);
	void ComputeFrac();
	void DoneFrac();
	void buildPalette();
	void Zoom(double z);
};

