/*********************************************************************************************
**
**	File Name:		burningship.cpp
**	Description:	This is the header file that contains the class function definitions for the
**					burning ship fractal
**
**	Author:			Clarke Needles
**	Created:		6/20/2025
**
**********************************************************************************************/

#include "burningship.h"

int BurningShip::getCPPIterF(float xval, float yval) const
{
    float x = 0.0, y = 0.0;
    float r = 0.0;
    int n = 0;

    // TODO: ADD A DESCIPTION OF WHAT THE BURNING SHIP EQUATION IS
    while (r < R_MAX && n < MAX_ITER) {
        float abs_x = x >= 0 ? x : -x;
        float abs_y = y >= 0 ? y : -y;

        float x2 = abs_x * abs_x;
        float y2 = abs_y * abs_y;
        r = x2 + y2; // Calculating r for quicker comparisons]

        x = x2 - y2 + xval;
        y = 2 * abs_x * abs_y + yval;

        ++n;
    }
    return n;
}

int BurningShip::getCPPIterD(double xval, double yval) const
{
    double x = 0.0, y = 0.0;
    double r = 0.0;
    int n = 0;

    // TODO: ADD A DESCIPTION OF WHAT THE BURNING SHIP EQUATION IS
    while (r < R_MAX && n < MAX_ITER) {
        double abs_x = x >= 0 ? x : -x;
        double abs_y = y >= 0 ? y : -y;

        // Saving a bit of time by doing less calculations
        // No need to recalculate each time
        double x2 = abs_x * abs_x;
        double y2 = abs_y * abs_y;
        r = x2 + y2; // Calculating r for quicker comparisons

        x = x2 - y2 + xval;
        y = 2 * abs_x * abs_y + yval;

        ++n;
    }
    return n;
}

__m128i BurningShip::getSSEIterF(__m128 xval, __m128 yval) const
{
    const __m128 r_max = _mm_set1_ps(R_MAX);
    const __m128 r0 = _mm_setzero_ps();
    const __m128 r1 = _mm_set1_ps(-1.0f);
    __m128i n = _mm_setzero_si128();
    __m128 x = _mm_setzero_ps();
    __m128 y = _mm_setzero_ps();
    __m128 abs_x = _mm_setzero_ps();
    __m128 abs_y = _mm_setzero_ps();
    __m128 r = _mm_setzero_ps();

    for (int i = 0; i < MAX_ITER; ++i)
    {
        __m128 cmp = _mm_cmp_ps(r_max, r, _CMP_GT_OQ); // if greater than the max r val, break
        if (!_mm_movemask_ps(cmp)) break;

        // Getting absolute value of x and y
        abs_x = _mm_andnot_ps(_mm_set1_ps(-0.0f), x); // Clearing the sign bit at the end
        abs_y = _mm_andnot_ps(_mm_set1_ps(-0.0f), y);

        __m128 xy = _mm_mul_ps(abs_x, abs_y);
        __m128 x2 = _mm_mul_ps(abs_x, abs_x);
        __m128 y2 = _mm_mul_ps(abs_y, abs_y);
        x = _mm_add_ps(_mm_sub_ps(x2, y2), xval); // Z real (zr*zr+zi*zi+cr)
        y = _mm_add_ps(_mm_add_ps(xy, xy), yval); // Z imag (zr*zi*2+ci)
        r = _mm_add_ps(x2, y2); // new R value (magnitude of Z) --> R <= R_max, zr^2+zi^2 <= R_max

        __m128i bn = _mm_castps_si128(cmp); // casting float cmp to an integer
        n = _mm_add_epi16(n, bn); // If cmp was previously true before hand, it will add 1 to N
    }

    return n;
}

__m128i BurningShip::getSSEIterD(__m128d xval, __m128d yval) const {
    const __m128d r_max = _mm_set1_pd(R_MAX);
    __m128i n = _mm_setzero_si128();
    __m128d x = _mm_setzero_pd();
    __m128d y = _mm_setzero_pd();
    __m128d abs_x = _mm_setzero_pd();
    __m128d abs_y = _mm_setzero_pd();
    __m128d r = _mm_setzero_pd();

    for (int i = 0; i < MAX_ITER; ++i) {
        __m128d cmp = _mm_cmp_pd(r_max, r, _CMP_GT_OQ);
        if (!_mm_movemask_pd(cmp)) break;

        abs_x = _mm_andnot_pd(_mm_set1_pd(-0.0f), x);
        abs_y = _mm_andnot_pd(_mm_set1_pd(-0.0f), y);

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

__m256i BurningShip::getAVXIterF(__m256 xval, __m256 yval) const {
    const __m256 r_max = _mm256_set1_ps(R_MAX);
    __m256i n = _mm256_setzero_si256();
    __m256 x = _mm256_setzero_ps();
    __m256 y = _mm256_setzero_ps();
    __m256 abs_x = _mm256_setzero_ps();
    __m256 abs_y = _mm256_setzero_ps();
    __m256 r = _mm256_setzero_ps();

    for (int i = 0; i < MAX_ITER; ++i) {
        __m256 cmp = _mm256_cmp_ps(r_max, r, _CMP_GT_OQ);
        if (!_mm256_movemask_ps(cmp)) break;

        abs_x = _mm256_andnot_ps(_mm256_set1_ps(-0.0f), x);
        abs_y = _mm256_andnot_ps(_mm256_set1_ps(-0.0f), y);

        __m256 xy = _mm256_mul_ps(abs_x, abs_y);
        __m256 x2 = _mm256_mul_ps(abs_x, abs_x);
        __m256 y2 = _mm256_mul_ps(abs_y, abs_y);
        x = _mm256_add_ps(_mm256_sub_ps(x2, y2), xval);
        y = _mm256_add_ps(_mm256_add_ps(xy, xy), yval);
        r = _mm256_add_ps(x2, y2);

        __m256i bn = _mm256_castps_si256(cmp);
        n = _mm256_add_epi16(n, bn);
    }

    return n;
}

__m256i BurningShip::getAVXIterD(__m256d xval, __m256d yval) const {
    const __m256d r_max = _mm256_set1_pd(R_MAX);
    __m256i n = _mm256_setzero_si256();
    __m256d x = _mm256_setzero_pd();
    __m256d y = _mm256_setzero_pd();
    __m256d abs_x = _mm256_setzero_pd();
    __m256d abs_y = _mm256_setzero_pd();
    __m256d r = _mm256_setzero_pd();

    for (int i = 0; i < MAX_ITER; ++i) {
        __m256d cmp = _mm256_cmp_pd(r_max, r, _CMP_GT_OQ);
        if (!_mm256_movemask_pd(cmp)) break;

        abs_x = _mm256_andnot_pd(_mm256_set1_pd(-0.0f), x);
        abs_y = _mm256_andnot_pd(_mm256_set1_pd(-0.0f), y);

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