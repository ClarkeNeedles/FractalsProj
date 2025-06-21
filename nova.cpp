/*********************************************************************************************
**
**	File Name:		nova.h
**	Description:	This is the header file that contains the class function definitions for the
**					nova fractal
**
**	Author:			Clarke Needles
**	Created:		6/20/2025
**
**********************************************************************************************/

#include "nova.h"

int Nova::getCPPIterF(float xval, float yval) const
{
    float x = 0.0, y = 0.0;
    float r = 0.0;
    int n = 0;

    while (r < R_MAX && n < MAX_ITER) {
        float x2 = x * x;
        float x3 = x2 * x;
        float y2 = y * y;
        float y3 = y2 * y;

        float fx = x3 - 3 * x * y2 - 1.0;
        float fy = 3 * x2 * y - y3;
        float fPx = 3.0f * (x2 - y2);
        float fPy = 6.0f * x * y;

        float denominator = fPx * fPx + fPy * fPy;
        float divx = 0.0f, divy = 0.0f;

        if (denominator > 1e-12f)
        {
            divx = (fx * fPx + fy * fPy) / denominator;
            divy = (fPx * fy - fx * fPy) / denominator;
        }

        r = x2 + y2;

        x = x - R_MAX * divx + xval;
        y = y - R_MAX * divy + yval;

        ++n;
    }

    return n;
}

int Nova::getCPPIterD(double xval, double yval) const
{
    double x = 0.0, y = 0.0;
    double r = 0.0;
    int n = 0;

    while (r < R_MAX && n < MAX_ITER) {
        double x2 = x * x;
        double x3 = x2 * x;
        double y2 = y * y;
        double y3 = y2 * y;

        double fx = x3 - 3 * x * y2 - 1.0;
        double fy = 3 * x2 * y - y3;
        double fPx = 3.0 * (x2 - y2);
        double fPy = 6.0 * x * y;

        double denominator = fPx * fPx + fPy * fPy;
        double divx = 0.0, divy = 0.0;

        if (denominator > 1e-12)
        {
            divx = (fx * fPx + fy * fPy) / denominator;
            divy = (fPx * fy - fx * fPy) / denominator;
        }

        r = x2 + y2;

        x = x - R_MAX * divx + xval;
        y = y - R_MAX * divy + yval;

        ++n;
    }

    return n;
}

__m128i Nova::getSSEIterF(__m128 xval, __m128 yval) const
{
    return _mm_setzero_si128();
}

__m128i Nova::getSSEIterD(__m128d xval, __m128d yval) const
{
    return _mm_setzero_si128();
}

__m256i Nova::getAVXIterF(__m256 xval, __m256 yval) const
{
    return _mm256_setzero_si256();
}

__m256i Nova::getAVXIterD(__m256d xval, __m256d yval) const
{
    return _mm256_setzero_si256();
}
