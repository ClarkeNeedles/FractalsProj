/*********************************************************************************************
**
**	File Name:		fractal.h
**	Description:	This is the header file that contains the abstract class for all
**                  of the fractals
**
**	Author:			Clarke Needles
**	Created:		5/23/2025
**
**********************************************************************************************/

#pragma once

#include <thread>
#include <string>
#include <Windows.h>
#include <functional>
#include <immintrin.h>
#include <emmintrin.h>
#include "winsettings.h"
#include "colour.h"
#include "gif.h"

// Fractal iterations
#define MAX_ITER 10000

// Escape boundary value
#define R_MAX 4.0

// SSE and AVX
// SSE uses 128 bit reg's --> fits 2 64 bit doubles, and 4 32 bit floats
// AVX uses 256 bit reg's --> fits 4 64 bit doubles, and 8 32 bit floats
#define VECTSIZE_SSE_D 2
#define VECTSIZE_SSE_F 4
#define VECTSIZE_AVX_D 4
#define VECTSIZE_AVX_F 8

// Switching condition float --> double
// When resolution gets low
#define F_TO_D 0.0001

// Zoom factor
#define ZF 1.5

enum zoomType
{
    Z_IN,
    Z_OUT
};

class Fractal
{
private:
    // Instance variables
    double xMin, xMax, yMin, yMax;


    // FOR RENDERING WITH CPP //
    
    // Determining iterations with CPP with floats
    virtual int getCPPIterF(float, float) const = 0;

    // Determining iterations with CPP with doubles
    virtual int getCPPIterD(double, double) const = 0;

    // Determining if a point is apart of the fractal in C++
    void useCPP(Colour* pixelBuffer, int yStart, int yEnd, std::wstring options[], bool useFloat);


    // FOR RENDERING WITH SSE //
    
    // Determining iterations with SSE with floats
    virtual __m128i getSSEIterF(__m128, __m128) const = 0;

    // Determining iterations with SSE with doubles
    virtual __m128i getSSEIterD(__m128d, __m128d) const = 0;

    // Determining if a point is apart of the fractal in SSE
    void useSSE(Colour* pixelBuffer, int yStart, int yEnd, std::wstring options[], bool useFloat);


    // FOR RENDERING WITH AVX //
    
    // Determining iterations with AVX with floats
    virtual __m256i getAVXIterF(__m256, __m256) const = 0;

    // Determining iterations with AVX with doubles
    virtual __m256i getAVXIterD(__m256d, __m256d) const = 0;

    // Determining if a point is apart of the fractal in AVX
    void useAVX(Colour* pixelBuffer, int yStart, int yEnd, std::wstring options[], bool useFloat);

    
    // HELPER FUNCTIONS //
    
    // Map iterations to a gradient
    void mapColour(Colour* pixelBuffer, uint8_t n, std::wstring gradient);

public:
    Fractal(double xMin, double xMax, double yMin, double yMax)
    {
        this->xMin = xMin;
        this->xMax = xMax;
        this->yMin = yMin;
        this->yMax = yMax;
    }

	// Virtual destructor
	virtual ~Fractal() {}

	// Function to render the fractal (May use multithreading depending on user selection)
    void render(Colour* pixelBuffer, std::wstring options[]);

    // Transferring the pixelBuffer bitmap to the main screen and writing to the gif
    void draw(HDC hdc, Colour* pixelBuffer, GifWriter* gif, bool recording);

    // Zooming in on the current fractal
    void zoomScreen(zoomType zoom);

    // Moving the screen on the current fractal
    void moveScreen(POINT* clickPoint);
};
