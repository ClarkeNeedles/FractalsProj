/*********************************************************************************************
**
**	File Name:		burningship.h
**	Description:	This is the header file that contains the class definition for the
**					burning ship fractal
**
**	Author:			Clarke Needles
**	Created:		5/23/2025
**
**********************************************************************************************/

#pragma once

#include "fractal.h"

// Initial complex plane size
#define BS_XMIN -2.2
#define BS_XMAX 1.4
#define BS_YMIN -2.1
#define BS_YMAX 0.9

class BurningShip : public Fractal
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
    BurningShip() : Fractal(BS_XMIN, BS_XMAX, BS_YMIN, BS_YMAX)
    {
    }

    // Destructor
    ~BurningShip() {}
};