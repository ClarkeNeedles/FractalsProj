/*********************************************************************************************
**
**	File Name:		pheonix.cpp
**	Description:	This is the header file that contains the class function definitions for the
**					pheonix fractal
**
**	Author:			Clarke Needles
**	Created:		6/20/2025
**
**********************************************************************************************/

#include "pheonix.h"

int Pheonix::getCPPIterF(float xval, float yval) const
{
    float px = -0.49f, py = 0.21f;
    float x = 0.0f, y = 0.0f;
    float xprev = 0.0f, yprev = 0.0f;
    float r = 0.0f;
    int n = 0;

    while (r < R_MAX && n < MAX_ITER)
    {
        float x2 = x * x;
        float y2 = y * y;
        float tempx = x2 - y2 + xval + px * xprev;
        float tempy = 2 * x * y + yval + py * yprev;

        xprev = x;
        yprev = y;
        r = x2 + y2;
        x = tempx;
        y = tempy;

        ++n;
    }

    return n;
}

int Pheonix::getCPPIterD(double xval, double yval) const
{
    double px = -0.49, py = 0.21;
    double x = 0.0, y = 0.0;
    double xprev = 0.0, yprev = 0.0;
    double r = 0.0;
    int n = 0;

    while (r < R_MAX && n < MAX_ITER)
    {
        double x2 = x * x;
        double y2 = y * y;
        double tempx = x2 - y2 + xval + px * xprev;
        double tempy = 2 * x * y + yval + py * yprev;

        xprev = x;
        yprev = y;
        r = x2 + y2;
        x = tempx;
        y = tempy;

        ++n;
    }

    return n;
}

__m128i Pheonix::getSSEIterF(__m128 xval, __m128 yval) const
{
    const __m128 r_max = _mm_set1_ps(R_MAX);
    const __m128 px = _mm_set1_ps(-0.49f);
    const __m128 py = _mm_set1_ps(0.21f);
    __m128i n = _mm_setzero_si128();
    __m128 x = _mm_setzero_ps();
    __m128 y = _mm_setzero_ps();
    __m128 xprev = _mm_setzero_ps();
    __m128 yprev = _mm_setzero_ps();
    __m128 r = _mm_setzero_ps();

    for (int i = 0; i < MAX_ITER; ++i)
    {
        __m128 cmp = _mm_cmp_ps(r_max, r, _CMP_GT_OQ);
        if (!_mm_movemask_ps(cmp)) break;

        __m128 x2 = _mm_mul_ps(x, x);
        __m128 y2 = _mm_mul_ps(y, y);
        __m128 pValx = _mm_mul_ps(px, xprev);
        __m128 pValy = _mm_mul_ps(py, yprev);

        __m128 xtemp = _mm_add_ps(pValx, xval);
        xtemp = _mm_add_ps(xtemp, _mm_sub_ps(x2, y2));

        __m128 ytemp = _mm_add_ps(pValy, yval);
        __m128 xy = _mm_mul_ps(x, y);
        ytemp = _mm_add_ps(ytemp, _mm_add_ps(xy, xy)); // 2xy

        xprev = x;
        yprev = y;

        r = _mm_add_ps(x2, y2);
        x = xtemp;
        y = ytemp;

        __m128i bn = _mm_castps_si128(cmp);
        n = _mm_add_epi16(n, bn);
    }

    return n;
}

__m128i Pheonix::getSSEIterD(__m128d xval, __m128d yval) const
{
    const __m128d r_max = _mm_set1_pd(R_MAX);
    const __m128d px = _mm_set1_pd(-0.49);
    const __m128d py = _mm_set1_pd(0.21);
    __m128i n = _mm_setzero_si128();
    __m128d x = _mm_setzero_pd();
    __m128d y = _mm_setzero_pd();
    __m128d xprev = _mm_setzero_pd();
    __m128d yprev = _mm_setzero_pd();
    __m128d r = _mm_setzero_pd();

    for (int i = 0; i < MAX_ITER; ++i)
    {
        __m128d cmp = _mm_cmp_pd(r_max, r, _CMP_GT_OQ);
        if (!_mm_movemask_pd(cmp)) break;

        __m128d x2 = _mm_mul_pd(x, x);
        __m128d y2 = _mm_mul_pd(y, y);
        __m128d pValx = _mm_mul_pd(px, xprev);
        __m128d pValy = _mm_mul_pd(py, yprev);

        __m128d xtemp = _mm_add_pd(pValx, xval);
        xtemp = _mm_add_pd(xtemp, _mm_sub_pd(x2, y2));

        __m128d ytemp = _mm_add_pd(pValy, yval);
        __m128d xy = _mm_mul_pd(x, y);
        ytemp = _mm_add_pd(ytemp, _mm_add_pd(xy, xy)); // 2xy

        xprev = x;
        yprev = y;

        r = _mm_add_pd(x2, y2);
        x = xtemp;
        y = ytemp;

        __m128i bn = _mm_castpd_si128(cmp);
        n = _mm_add_epi16(n, bn);
    }

    return n;
}

__m256i Pheonix::getAVXIterF(__m256 xval, __m256 yval) const
{
    const __m256 r_max = _mm256_set1_ps(R_MAX);
    const __m256 px = _mm256_set1_ps(-0.49f);
    const __m256 py = _mm256_set1_ps(0.21f);
    __m256i n = _mm256_setzero_si256();
    __m256 x = _mm256_setzero_ps();
    __m256 y = _mm256_setzero_ps();
    __m256 xprev = _mm256_setzero_ps();
    __m256 yprev = _mm256_setzero_ps();
    __m256 r = _mm256_setzero_ps();

    for (int i = 0; i < MAX_ITER; ++i)
    {
        __m256 cmp = _mm256_cmp_ps(r_max, r, _CMP_GT_OQ);
        if (!_mm256_movemask_ps(cmp)) break;

        __m256 x2 = _mm256_mul_ps(x, x);
        __m256 y2 = _mm256_mul_ps(y, y);
        __m256 pValx = _mm256_mul_ps(px, xprev);
        __m256 pValy = _mm256_mul_ps(py, yprev);

        __m256 xtemp = _mm256_add_ps(pValx, xval);
        xtemp = _mm256_add_ps(xtemp, _mm256_sub_ps(x2, y2));

        __m256 ytemp = _mm256_add_ps(pValy, yval);
        __m256 xy = _mm256_mul_ps(x, y);
        ytemp = _mm256_add_ps(ytemp, _mm256_add_ps(xy, xy)); // 2xy

        xprev = x;
        yprev = y;

        r = _mm256_add_ps(x2, y2);
        x = xtemp;
        y = ytemp;

        __m256i bn = _mm256_castps_si256(cmp);
        n = _mm256_add_epi16(n, bn);
    }

    return n;
}

__m256i Pheonix::getAVXIterD(__m256d xval, __m256d yval) const
{
    const __m256d r_max = _mm256_set1_pd(R_MAX);
    const __m256d px = _mm256_set1_pd(-0.49);
    const __m256d py = _mm256_set1_pd(0.21);
    __m256i n = _mm256_setzero_si256();
    __m256d x = _mm256_setzero_pd();
    __m256d y = _mm256_setzero_pd();
    __m256d xprev = _mm256_setzero_pd();
    __m256d yprev = _mm256_setzero_pd();
    __m256d r = _mm256_setzero_pd();

    for (int i = 0; i < MAX_ITER; ++i)
    {
        __m256d cmp = _mm256_cmp_pd(r_max, r, _CMP_GT_OQ);
        if (!_mm256_movemask_pd(cmp)) break;

        __m256d x2 = _mm256_mul_pd(x, x);
        __m256d y2 = _mm256_mul_pd(y, y);
        __m256d pValx = _mm256_mul_pd(px, xprev);
        __m256d pValy = _mm256_mul_pd(py, yprev);

        __m256d xtemp = _mm256_add_pd(pValx, xval);
        xtemp = _mm256_add_pd(xtemp, _mm256_sub_pd(x2, y2));

        __m256d ytemp = _mm256_add_pd(pValy, yval);
        __m256d xy = _mm256_mul_pd(x, y);
        ytemp = _mm256_add_pd(ytemp, _mm256_add_pd(xy, xy)); // 2xy

        xprev = x;
        yprev = y;

        r = _mm256_add_pd(x2, y2);
        x = xtemp;
        y = ytemp;

        __m256i bn = _mm256_castpd_si256(cmp);
        n = _mm256_add_epi16(n, bn);
    }

    return n;
}
