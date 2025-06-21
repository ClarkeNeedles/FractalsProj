/*********************************************************************************************
**
**	File Name:		multibrot.cpp
**	Description:	This is the header file that contains the class function definitions for the
**					mandelbrot with higher order set
**
**	Author:			Clarke Needles
**	Created:		6/20/2025
**
**********************************************************************************************/

#include "multibrot.h"

int Multibrot::getCPPIterF(float xval, float yval) const
{
    float x = 0.0, y = 0.0;
    float r = 0.0;
    int n = 0;

    while (r < R_MAX && n < MAX_ITER) {
        float x2 = x * x;
        float x3 = x2 * x;
        float x4 = x3 * x;
        float x5 = x4 * x;

        float y2 = y * y;
        float y3 = y2 * y;
        float y4 = y3 * y;
        float y5 = y4 * y;

        r = x2 + y2;

        x = x5 - 10 * x3 * y2 + 5 * x * y4 + xval;
        y = 5 * x4 * y - 10 * x2 * y3 + y5 + yval;

        ++n;
    }

    return n;
}

int Multibrot::getCPPIterD(double xval, double yval) const
{
    double x = 0.0, y = 0.0;
    double r = 0.0;
    int n = 0;

    while (r < R_MAX && n < MAX_ITER) {
        double x2 = x * x;
        double x3 = x2 * x;
        double x4 = x3 * x;
        double x5 = x4 * x;

        double y2 = y * y;
        double y3 = y2 * y;
        double y4 = y3 * y;
        double y5 = y4 * y;

        r = x2 + y2;

        x = x5 - 10 * x3 * y2 + 5 * x * y4 + xval;
        y = 5 * x4 * y - 10 * x2 * y3 + y5 + yval;

        ++n;
    }

    return n;
}

__m128i Multibrot::getSSEIterF(__m128 xval, __m128 yval) const
{
    const __m128 r_max = _mm_set1_ps(R_MAX);
    __m128i n = _mm_setzero_si128();
    __m128 x = _mm_setzero_ps();
    __m128 y = _mm_setzero_ps();
    __m128 r = _mm_setzero_ps();

    for (int i = 0; i < MAX_ITER; ++i)
    {
        __m128 cmp = _mm_cmp_ps(r_max, r, _CMP_GT_OQ);
        if (!_mm_movemask_ps(cmp)) break;

        __m128 x2 = _mm_mul_ps(x, x);
        __m128 x3 = _mm_mul_ps(x2, x);
        __m128 x4 = _mm_mul_ps(x3, x);
        __m128 x5 = _mm_mul_ps(x4, x);

        __m128 y2 = _mm_mul_ps(y, y);
        __m128 y3 = _mm_mul_ps(y2, y);
        __m128 y4 = _mm_mul_ps(y3, y);
        __m128 y5 = _mm_mul_ps(y4, y);

        __m128 real1 = _mm_mul_ps(_mm_mul_ps(_mm_set1_ps(10), x3), y2); // 10x^3y^2
        __m128 real2 = _mm_mul_ps(_mm_mul_ps(_mm_set1_ps(5), x), y4);   // 5xy^4
        x = _mm_add_ps(_mm_add_ps(_mm_sub_ps(x5, real1), real2), xval);

        __m128 imag1 = _mm_mul_ps(_mm_mul_ps(_mm_set1_ps(5), x4), y);
        __m128 imag2 = _mm_mul_ps(_mm_mul_ps(_mm_set1_ps(10), x2), y3);
        y = _mm_add_ps(_mm_add_ps(_mm_sub_ps(imag1, imag2), y5), yval);

        r = _mm_add_ps(x2, y2);

        __m128i bn = _mm_castps_si128(cmp);
        n = _mm_add_epi16(n, bn);
    }

    return n;
}

__m128i Multibrot::getSSEIterD(__m128d xval, __m128d yval) const
{
    const __m128d r_max = _mm_set1_pd(R_MAX);
    __m128i n = _mm_setzero_si128();
    __m128d x = _mm_setzero_pd();
    __m128d y = _mm_setzero_pd();
    __m128d abs_x = _mm_setzero_pd();
    __m128d abs_y = _mm_setzero_pd();
    __m128d r = _mm_setzero_pd();

    for (int i = 0; i < MAX_ITER; ++i)
    {
        __m128d cmp = _mm_cmp_pd(r_max, r, _CMP_GT_OQ);
        if (!_mm_movemask_pd(cmp)) break;

        abs_x = _mm_andnot_pd(_mm_set1_pd(-0.0), x);
        abs_y = _mm_andnot_pd(_mm_set1_pd(-0.0), y);

        __m128d xy = _mm_mul_pd(abs_x, abs_y);
        __m128d x2 = _mm_mul_pd(abs_x, abs_x);
        __m128d y2 = _mm_mul_pd(abs_y, abs_y);
        x = _mm_add_pd(_mm_sub_pd(x2, y2), xval);
        y = _mm_add_pd(_mm_add_pd(xy, xy), yval);
        r = _mm_add_pd(x2, y2);

        __m128i bn = _mm_castpd_si128(cmp);
        n = _mm_add_epi16(n, bn);
    }

    return n;
}

__m256i Multibrot::getAVXIterF(__m256 xval, __m256 yval) const
{
    const __m256 r_max = _mm256_set1_ps(R_MAX);
    __m256i n = _mm256_setzero_si256();
    __m256 x = _mm256_setzero_ps();
    __m256 y = _mm256_setzero_ps();
    __m256 r = _mm256_setzero_ps();

    for (int i = 0; i < MAX_ITER; ++i)
    {
        __m256 cmp = _mm256_cmp_ps(r_max, r, _CMP_GT_OQ);
        if (!_mm256_movemask_ps(cmp)) break;

        __m256 x2 = _mm256_mul_ps(x, x);
        __m256 x3 = _mm256_mul_ps(x2, x);
        __m256 x4 = _mm256_mul_ps(x3, x);
        __m256 x5 = _mm256_mul_ps(x4, x);

        __m256 y2 = _mm256_mul_ps(y, y);
        __m256 y3 = _mm256_mul_ps(y2, y);
        __m256 y4 = _mm256_mul_ps(y3, y);
        __m256 y5 = _mm256_mul_ps(y4, y);

        __m256 real1 = _mm256_mul_ps(_mm256_mul_ps(_mm256_set1_ps(10), x3), y2);
        __m256 real2 = _mm256_mul_ps(_mm256_mul_ps(_mm256_set1_ps(5), x), y4);
        x = _mm256_add_ps(_mm256_add_ps(_mm256_sub_ps(x5, real1), real2), xval);

        __m256 imag1 = _mm256_mul_ps(_mm256_mul_ps(_mm256_set1_ps(5), x4), y);
        __m256 imag2 = _mm256_mul_ps(_mm256_mul_ps(_mm256_set1_ps(10), x2), y3);
        y = _mm256_add_ps(_mm256_add_ps(_mm256_sub_ps(imag1, imag2), y5), yval);

        r = _mm256_add_ps(x2, y2);

        __m256i bn = _mm256_castps_si256(cmp);
        n = _mm256_add_epi16(n, bn);
    }

    return n;
}

__m256i Multibrot::getAVXIterD(__m256d xval, __m256d yval) const
{
    const __m256d r_max = _mm256_set1_pd(R_MAX);
    __m256i n = _mm256_setzero_si256();
    __m256d x = _mm256_setzero_pd();
    __m256d y = _mm256_setzero_pd();
    __m256d abs_x = _mm256_setzero_pd();
    __m256d abs_y = _mm256_setzero_pd();
    __m256d r = _mm256_setzero_pd();

    for (int i = 0; i < MAX_ITER; ++i)
    {
        __m256d cmp = _mm256_cmp_pd(r_max, r, _CMP_GT_OQ);
        if (!_mm256_movemask_pd(cmp)) break;

        abs_x = _mm256_andnot_pd(_mm256_set1_pd(-0.0), x);
        abs_y = _mm256_andnot_pd(_mm256_set1_pd(-0.0), y);

        __m256d xy = _mm256_mul_pd(abs_x, abs_y);
        __m256d x2 = _mm256_mul_pd(abs_x, abs_x);
        __m256d y2 = _mm256_mul_pd(abs_y, abs_y);
        x = _mm256_add_pd(_mm256_sub_pd(x2, y2), xval);
        y = _mm256_add_pd(_mm256_add_pd(xy, xy), yval);
        r = _mm256_add_pd(x2, y2);

        __m256i bn = _mm256_castpd_si256(cmp);
        n = _mm256_add_epi16(n, bn);
    }

    return n;
}
