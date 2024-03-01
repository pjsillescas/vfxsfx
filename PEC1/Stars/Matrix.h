#pragma once

#include "Vector.h"

class MATRIX
{

	VECTOR m[3];

public:

	VECTOR& operator[](const int i);
	const VECTOR& operator[](const int i) const;


	MATRIX operator*(const MATRIX& b) const;

	MATRIX(const double a11, const double a12, const double a13,
		const double a21, const double a22, const double a23,
		const double a31, const double a32, const double a33);

	VECTOR operator*(const VECTOR& v) const;

	MATRIX();
	~MATRIX();
};

MATRIX rotX(const double theta);
MATRIX rotY(const double theta);
MATRIX rotZ(const double theta);
