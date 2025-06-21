/*********************************************************************************************
**
**	File Name:		multibrot.h
**	Description:	This is the header file that contains the class definition for the
**					mandelbrot with higher order set
**
**	Author:			Clarke Needles
**	Created:		5/26/2025
**
**********************************************************************************************/

#pragma once

#include "fractal.h"

// Initial complex plane size
#define MULB_XMIN -1.5
#define MULB_XMAX 1.5
#define MULB_YMIN -1.5
#define MULB_YMAX 1.5

class Multibrot : public Fractal
{
private:
    int getCPPIterF(float xval, float yval) const override;

    int getCPPIterD(double xval, double yval) const override;

    __m128i getSSEIterF(__m128 xval, __m128 yval) const override;

    __m128i getSSEIterD(__m128d xval, __m128d yval) const override;

    __m256i getAVXIterF(__m256 xval, __m256 yval) const override;

    __m256i getAVXIterD(__m256d xval, __m256d yval) const override;

public:
    // Constructor
    Multibrot() : Fractal(MULB_XMIN, MULB_XMAX, MULB_YMIN, MULB_YMAX)
    {
    }

    // Destructor
    ~Multibrot() {}
};
