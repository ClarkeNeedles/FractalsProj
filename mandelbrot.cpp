/*********************************************************************************************
**
**	File Name:		mandelbrot.cpp
**	Description:	This is the header file that contains the class function definitions for the
**                  mandelbrot fractal
**
**	Author:			Clarke Needles
**	Created:		6/20/2025
**
**********************************************************************************************/

#include "mandelbrot.h"

int Mandelbrot::getCPPIterF(float xval, float yval) const
{
    float x = 0.0, y = 0.0;
    float r = 0;
    int n = 0;

    while (r < R_MAX && n < MAX_ITER)
    {
        float x2 = x * x;
        float y2 = y * y;
        float temp = x2 - y2 + xval;
        r = x2 + y2;

        y = 2 * x * y + yval;
        x = temp;
        ++n;
    }

    return n;
}

int Mandelbrot::getCPPIterD(double xval, double yval) const
{
    double x = 0.0, y = 0.0;
    double r = 0;
    int n = 0;

    while (r < R_MAX && n < MAX_ITER)
    {
        double x2 = x * x;
        double y2 = y * y;
        double temp = x2 - y2 + xval;
        r = x2 + y2;

        y = 2 * x * y + yval;
        x = temp;
        ++n;
    }
    return n;
}

__m128i Mandelbrot::getSSEIterF(__m128 xval, __m128 yval) const
{
    const __m128 r_max = _mm_set1_ps(R_MAX);
    __m128i n = _mm_setzero_si128();
    __m128 x = _mm_setzero_ps();
    __m128 y = _mm_setzero_ps();
    __m128 x2 = _mm_setzero_ps();
    __m128 y2 = _mm_setzero_ps();
    __m128 xy = _mm_setzero_ps();
    __m128 r = _mm_setzero_ps();

    for (int i = 0; i < MAX_ITER; ++i)
    {
        __m128 cmp = _mm_cmp_ps(r_max, r, _CMP_GT_OQ);
        if (!_mm_movemask_ps(cmp)) break;

        x2 = _mm_mul_ps(x, x);
        y2 = _mm_mul_ps(y, y);
        xy = _mm_mul_ps(x, y);
        x = _mm_add_ps(_mm_sub_ps(x2, y2), xval);
        y = _mm_add_ps(_mm_add_ps(xy, xy), yval);
        r = _mm_add_ps(x2, y2);

        __m128i bn = _mm_castps_si128(cmp);
        n = _mm_add_epi16(n, bn);
    }

    return n;
}

__m128i Mandelbrot::getSSEIterD(__m128d xval, __m128d yval) const
{
    const __m128d r_max = _mm_set1_pd(R_MAX);
    __m128i n = _mm_setzero_si128();
    __m128d x = _mm_setzero_pd();
    __m128d y = _mm_setzero_pd();
    __m128d x2 = _mm_setzero_pd();
    __m128d y2 = _mm_setzero_pd();
    __m128d xy = _mm_setzero_pd();
    __m128d r = _mm_setzero_pd();

    for (int i = 0; i < MAX_ITER; ++i)
    {
        __m128d cmp = _mm_cmp_pd(r_max, r, _CMP_GT_OQ);
        if (!_mm_movemask_pd(cmp)) break;

        x2 = _mm_mul_pd(x, x);
        y2 = _mm_mul_pd(y, y);
        xy = _mm_mul_pd(x, y);
        x = _mm_add_pd(_mm_sub_pd(x2, y2), xval);
        y = _mm_add_pd(_mm_add_pd(xy, xy), yval);
        r = _mm_add_pd(x2, y2);

        __m128i bn = _mm_castpd_si128(cmp);
        n = _mm_add_epi16(n, bn);
    }
    return n;
}

__m256i Mandelbrot::getAVXIterF(__m256 xval, __m256 yval) const
{
    const __m256 r_max = _mm256_set1_ps(R_MAX);
    __m256i n = _mm256_setzero_si256();
    __m256 x = _mm256_setzero_ps();
    __m256 y = _mm256_setzero_ps();
    __m256 x2 = _mm256_setzero_ps();
    __m256 y2 = _mm256_setzero_ps();
    __m256 xy = _mm256_setzero_ps();
    __m256 r = _mm256_setzero_ps();

    for (int i = 0; i < MAX_ITER; ++i)
    {
        __m256 cmp = _mm256_cmp_ps(r_max, r, _CMP_GT_OQ);
        if (!_mm256_movemask_ps(cmp)) break;

        x2 = _mm256_mul_ps(x, x);
        y2 = _mm256_mul_ps(y, y);
        xy = _mm256_mul_ps(x, y);
        x = _mm256_add_ps(_mm256_sub_ps(x2, y2), xval);
        y = _mm256_add_ps(_mm256_add_ps(xy, xy), yval);
        r = _mm256_add_ps(x2, y2);

        __m256i bn = _mm256_castps_si256(cmp);
        n = _mm256_add_epi16(n, bn);
    }

    return n;
}

__m256i Mandelbrot::getAVXIterD(__m256d xval, __m256d yval) const
{
    const __m256d r_max = _mm256_set1_pd(R_MAX);
    __m256i n = _mm256_setzero_si256();
    __m256d x = _mm256_setzero_pd();
    __m256d y = _mm256_setzero_pd();
    __m256d x2 = _mm256_setzero_pd();
    __m256d y2 = _mm256_setzero_pd();
    __m256d xy = _mm256_setzero_pd();
    __m256d r = _mm256_setzero_pd();

    for (int i = 0; i < MAX_ITER; ++i)
    {
        __m256d cmp = _mm256_cmp_pd(r_max, r, _CMP_GT_OQ);
        if (!_mm256_movemask_pd(cmp)) break;

        x2 = _mm256_mul_pd(x, x);
        y2 = _mm256_mul_pd(y, y);
        xy = _mm256_mul_pd(x, y);
        x = _mm256_add_pd(_mm256_sub_pd(x2, y2), xval);
        y = _mm256_add_pd(_mm256_add_pd(xy, xy), yval);
        r = _mm256_add_pd(x2, y2);

        __m256i bn = _mm256_castpd_si256(cmp);
        n = _mm256_add_epi16(n, bn);
    }

    return n;
}
