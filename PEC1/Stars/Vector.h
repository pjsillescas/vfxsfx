#pragma once

#include <cmath>

class VECTOR
{
    float v[3];

public:

    float& operator[](const int i);
    const float& operator[](const int i) const;

    VECTOR operator+(const VECTOR& a) const;
    VECTOR operator-(const VECTOR& a) const;
    VECTOR& operator=(const VECTOR& a);

    VECTOR();
    VECTOR(const float X, const float Y, const float Z);
    ~VECTOR();
};

VECTOR normalize(const VECTOR& a);
