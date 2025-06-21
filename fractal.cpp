/*********************************************************************************************
**
**	File Name:		fractal.cpp
**	Description:	This is the header file that contains the function definiton for all of 
**					the fractals.
**
**	Author:			Clarke Needles
**	Created:		6/20/2025
**
**********************************************************************************************/

#include "fractal.h"

void Fractal::useCPP(Colour* pixelBuffer, int yStart, int yEnd, std::wstring options[], bool useFloat)
{
    // Variables for updating the x and y values
    // Essentially mapping a complex plane point to a pixel
    double dx = (xMax - xMin) / static_cast<double>(WIDTHW);
    double dy = (yMax - yMin) / static_cast<double>(HEIGHTW);

    double yval = yMin + yStart * dy;
    for (int y = yStart; y < yEnd; ++y)
    {
        double xval = xMin;
        for (int x = 0; x < WIDTHW; ++x)
        {
            int n;
            if (useFloat)
            {
                n = getCPPIterF(static_cast<float>(xval), static_cast<float>(yval));
            }
            else
            {
                n = getCPPIterD(xval, yval);
            }

            mapColour(&pixelBuffer[y * WIDTHW + x], static_cast<uint8_t>(n), options[2]);
            xval += dx;
        }
        yval += dy;
    }
}

void Fractal::useSSE(Colour* pixelBuffer, int yStart, int yEnd, std::wstring options[], bool useFloat)
{
    if (useFloat)
    {
        float dx = (xMax - xMin) / static_cast<float>(WIDTHW);
        float dy = (yMax - yMin) / static_cast<float>(HEIGHTW);
        const __m128 xShift_coeffs = _mm_set_ps(3.0f, 2.0f, 1.0f, 0.0f);
        __m128 yval = _mm_add_ps(_mm_set1_ps(yMin), _mm_mul_ps(_mm_set1_ps(yStart), _mm_set1_ps(dy))); // Setting initial yval to startin range of y
        __m128 xShift = _mm_mul_ps(_mm_set1_ps(dx), xShift_coeffs); // Amount to shift x for each number we will be processing
        __m128 dxSSE = _mm_set1_ps(VECTSIZE_SSE_F * dx); // Amount to change multiplied by the number of floats calculated in parallel
        __m128 dySSE = _mm_set1_ps(dy); // The change for y each time will be the default

        for (int y = yStart; y < yEnd; ++y)
        {
            __m128 xval = _mm_add_ps(_mm_set1_ps(xMin), xShift); // Initial x values for first floats

            for (int x = 0; x < WIDTHW; x += VECTSIZE_SSE_F) // Increase by the amount of floats being processed each time
            {
                __m128i iter = getSSEIterF(xval, yval); // Calculate amount of iterations for the floats

                int* n_int = (int*)(&iter); // Pointer to the number of iterations
                int pixel_i = y * WIDTHW + x; // Current pixel index

                // Colour the pixels that are loaded
                for (int i = 0; i < VECTSIZE_SSE_F; ++i, ++pixel_i)
                {
                    uint8_t n = (uint8_t)(n_int[i]); // Changing the pointer to unsigned int
                    mapColour(&pixelBuffer[pixel_i], n, options[2]);
                }

                xval = _mm_add_ps(xval, dxSSE); // Updating the x values
            }

            yval = _mm_add_ps(yval, dySSE); // Updating the y value
        }
    }
    else
    {
        double dx = (xMax - xMin) / static_cast<double>(WIDTHW);
        double dy = (yMax - yMin) / static_cast<double>(HEIGHTW);
        __m128d xShift_coeffs = _mm_set_pd(1.0, 0.0); // 2 doubles
        __m128d yval = _mm_add_pd(_mm_set1_pd(yMin), _mm_mul_pd(_mm_set1_pd(yStart), _mm_set1_pd(dy))); // Setting initial yval
        __m128d xShift = _mm_mul_pd(_mm_set1_pd(dx), xShift_coeffs); // Amount to shift x for each pair of numbers
        __m128d dxSSE = _mm_set1_pd(VECTSIZE_SSE_D * dx); // Amount to change for 2 doubles at a time
        __m128d dySSE = _mm_set1_pd(dy); // The change in y is the default value

        for (int y = yStart; y < yEnd; ++y) {
            __m128d xval = _mm_add_pd(_mm_set1_pd(xMin), xShift); // Initial x values for the first 2 doubles

            for (int x = 0; x < WIDTHW; x += VECTSIZE_SSE_D) { // Increase by the number of doubles being processed per iteration
                __m128i iter = getSSEIterD(xval, yval); // Calculate iterations for the doubles

                int* n_int = (int*)(&iter); // Pointer to the number of iterations
                int pixel_i = y * WIDTHW + x; // Current pixel index

                // Colour the pixels for the doubles being calculated in parallel
                for (int i = 0; i < VECTSIZE_SSE_D; ++i, ++pixel_i) {
                    uint8_t n = (uint8_t)(n_int[i]); // Changing the pointer to unsigned int
                    mapColour(&pixelBuffer[pixel_i], n, options[2]);
                }

                xval = _mm_add_pd(xval, dxSSE); // Update the x values
            }

            yval = _mm_add_pd(yval, dySSE); // Update the y value
        }
    }
}

void Fractal::useAVX(Colour* pixelBuffer, int yStart, int yEnd, std::wstring options[], bool useFloat)
{
    if (useFloat)
    {
        float dx = (xMax - xMin) / static_cast<float>(WIDTHW);
        float dy = (yMax - yMin) / static_cast<float>(HEIGHTW);
        const __m256 xShift_coeffs = _mm256_set_ps(7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f, 0.0f); // Shifting coefficients values
        __m256 yval = _mm256_add_ps(_mm256_set1_ps(yMin), _mm256_mul_ps(_mm256_set1_ps(yStart), _mm256_set1_ps(dy))); // Setting initial yval to startin range of y
        __m256 xShift = _mm256_mul_ps(_mm256_set1_ps(dx), xShift_coeffs); // Amount to shift x for each number we will be processing
        __m256 dxSSE = _mm256_set1_ps(VECTSIZE_AVX_F * dx); // Amount to change multiplied by the number of floats calculated in parallel
        __m256 dySSE = _mm256_set1_ps(dy); // The change for y each time will be the default

        for (int y = yStart; y < yEnd; ++y)
        {
            __m256 xval = _mm256_add_ps(_mm256_set1_ps(xMin), xShift); // Initial x values for first floats

            for (int x = 0; x < WIDTHW; x += VECTSIZE_AVX_F) // Increase by the amount of floats being processed each time
            {
                __m256i N = getAVXIterF(xval, yval); // Calculate amount of iterations for the floats

                int* N_int = (int*)(&N); // Pointer to the number of iterations
                int pixel_i = y * WIDTHW + x; // Current pixel index

                // Colour the pixels that are loaded
                for (int i = 0; i < VECTSIZE_AVX_F; ++i, ++pixel_i)
                {
                    uint8_t n = (uint8_t)(N_int[i]); // Changing the pointer to unsigned int
                    mapColour(&pixelBuffer[pixel_i], n, options[2]);
                }

                xval = _mm256_add_ps(xval, dxSSE); // Updating the x values
            }

            yval = _mm256_add_ps(yval, dySSE); // Updating the y value
        }
    }
    else
    {
        double dx = (xMax - xMin) / static_cast<double>(WIDTHW);
        double dy = (yMax - yMin) / static_cast<double>(HEIGHTW);
        const __m256d xShift_coeffs = _mm256_set_pd(3.0, 2.0, 1.0, 0.0); // Shifting coefficients values
        __m256d yval = _mm256_add_pd(_mm256_set1_pd(yMin), _mm256_mul_pd(_mm256_set1_pd(yStart), _mm256_set1_pd(dy))); // Setting initial yval to startin range of y
        __m256d xShift = _mm256_mul_pd(_mm256_set1_pd(dx), xShift_coeffs); // Amount to shift x for each number we will be processing
        __m256d dxSSE = _mm256_set1_pd(VECTSIZE_AVX_D * dx); // Amount to change multiplied by the number of floats calculated in parallel
        __m256d dySSE = _mm256_set1_pd(dy); // The change for y each time will be the default

        for (int y = yStart; y < yEnd; ++y)
        {
            __m256d xval = _mm256_add_pd(_mm256_set1_pd(xMin), xShift); // Initial x values for first floats

            for (int x = 0; x < WIDTHW; x += VECTSIZE_AVX_D) // Increase by the amount of floats being processed each time
            {
                __m256i N = getAVXIterD(xval, yval); // Calculate amount of iterations for the floats

                int* N_int = (int*)(&N); // Pointer to the number of iterations
                int pixel_i = y * WIDTHW + x; // Current pixel index

                // Colour the pixels that are loaded
                for (int i = 0; i < VECTSIZE_AVX_D; ++i, ++pixel_i)
                {
                    uint8_t n = (uint8_t)(N_int[i]); // Changing the pointer to unsigned int
                    mapColour(&pixelBuffer[pixel_i], n, options[2]);
                }

                xval = _mm256_add_pd(xval, dxSSE); // Updating the x values
            }

            yval = _mm256_add_pd(yval, dySSE); // Updating the y value
        }
    }
}

void Fractal::mapColour(Colour* pixelBuffer, uint8_t n, std::wstring gradient) 
{
    // Color mapping for points outside of the set
    // The gradient depends on the combobox option
    if (gradient._Equal(comboboxes[2].options[0]))
    {
        pixelBuffer->r = (n * 13) % 256;
        pixelBuffer->g = (n * 7) % 256;
        pixelBuffer->b = (n * 3) % 256;
        pixelBuffer->a = 0;
    }
    else if (gradient._Equal(comboboxes[2].options[1]))
    {
        pixelBuffer->r = (n * 3) % 256;
        pixelBuffer->g = (n * 7) % 256;
        pixelBuffer->b = (n * 13) % 256;
        pixelBuffer->a = 0;
    }
    else if (gradient._Equal(comboboxes[2].options[2]))
    {
        pixelBuffer->r = (int)(127.5 * (1 + sin(n * 0.1)));
        pixelBuffer->g = (int)(127.5 * (1 + sin(n * 0.15)));
        pixelBuffer->b = (int)(127.5 * (1 + sin(n * 0.2)));
        pixelBuffer->a = 0;
    }
    else if (gradient._Equal(comboboxes[2].options[3]))
    {
        pixelBuffer->r = (int)(sqrt(n * 10)) % 256; // Slower ramp-up for red
        pixelBuffer->g = (int)(log(n + 1) * 50) % 256; // Logarithmic ramp-up for green
        pixelBuffer->b = (n * n) % 256;
        pixelBuffer->a = 0;
    }
    else if (gradient._Equal(comboboxes[2].options[4]))
    {
        if (n < 50) {
            pixelBuffer->r = (n * 3) % 256;
            pixelBuffer->g = (n * 2) % 256;
            pixelBuffer->b = 0;
            pixelBuffer->a = 0;
        }
        else if (n < 100) {
            pixelBuffer->r = 0;
            pixelBuffer->g = (n * 4) % 256;
            pixelBuffer->b = (n * 6) % 256;
            pixelBuffer->a = 0;
        }
        else {
            pixelBuffer->r = (n * 8) % 256;
            pixelBuffer->g = (n * 3) % 256;
            pixelBuffer->b = (n * 1) % 256;
            pixelBuffer->a = 0;
        }
    }
    else if (gradient._Equal(comboboxes[2].options[5]))
    {
        int r = (n * n) % 256;
        int g = (n * 3 + 50) % 256;
        int b = (n * 7 - 30) % 256;
        int a = 0;
        pixelBuffer->r = (r + b) % 256;
        pixelBuffer->g = (g + r) % 256;
        pixelBuffer->b = (b + g) % 256;
        pixelBuffer->a = 0;
    }
    else
    {
        pixelBuffer->r = (255 - ((n * 1) % 256));
        pixelBuffer->g = (255 - ((n * 5) % 256));
        pixelBuffer->b = (255 - ((n * 10) % 256));
        pixelBuffer->a = 0;
    }
}

void Fractal::render(Colour* pixelBuffer, std::wstring options[]) 
{
    // Dynamically changing from float to double when resolution gets low
    bool useDouble = yMax - yMin < F_TO_D;

    // Use multithreading
    if (options[0]._Equal(comboboxes[0].options[3]) ||
        options[0]._Equal(comboboxes[0].options[4]) ||
        options[0]._Equal(comboboxes[0].options[5]))
    {
        int numThreads;
        int cores = std::thread::hardware_concurrency(); // Number of available CPU cores
        if (options[0]._Equal(comboboxes[0].options[5]))
        {
            // Hard coded value that actually speeds up AVX Multithreaded
            // Using to many cores is uncessary and slows it down, 8 is perfect
            numThreads = cores < 8 ? cores : 8;
        }

        numThreads = cores;
        int stripHeight = HEIGHTW / numThreads;
        std::vector<std::thread> threads;

        // Assign a thread to each strip
        for (int i = 0; i < numThreads; ++i) {
            int yStart = i * stripHeight;

            if (useDouble)
            {
                if (options[0]._Equal(comboboxes[0].options[3]))
                {
                    threads.emplace_back(std::bind(&Fractal::useCPP, this, pixelBuffer, yStart, yStart + stripHeight, options, false));
                }
                else if (options[0]._Equal(comboboxes[0].options[4]))
                {
                    threads.emplace_back(std::bind(&Fractal::useSSE, this, pixelBuffer, yStart, yStart + stripHeight, options, false));
                }
                else
                {
                    threads.emplace_back(std::bind(&Fractal::useAVX, this, pixelBuffer, yStart, yStart + stripHeight, options, false));
                }
            }
            else
            {
                if (options[0]._Equal(comboboxes[0].options[3]))
                {
                    threads.emplace_back(std::bind(&Fractal::useCPP, this, pixelBuffer, yStart, yStart + stripHeight, options, true));
                }
                else if (options[0]._Equal(comboboxes[0].options[4]))
                {
                    threads.emplace_back(std::bind(&Fractal::useSSE, this, pixelBuffer, yStart, yStart + stripHeight, options, true));
                }
                else
                {
                    threads.emplace_back(std::bind(&Fractal::useAVX, this, pixelBuffer, yStart, yStart + stripHeight, options, true));
                }
            }
        }

        // Wait for all threads to complete
        for (auto& t : threads) {
            t.join();
        }
    }
    else
    {
        if (useDouble)
        {
            if (options[0]._Equal(comboboxes[0].options[0]))
            {
                useCPP(pixelBuffer, 0, HEIGHTW, options, false);
            }
            else if (options[0]._Equal(comboboxes[0].options[1]))
            {
                useSSE(pixelBuffer, 0, HEIGHTW, options, false);
            }
            else
            {
                useAVX(pixelBuffer, 0, HEIGHTW, options, false);
            }
        }
        else
        {
            if (options[0]._Equal(comboboxes[0].options[0]))
            {
                useCPP(pixelBuffer, 0, HEIGHTW, options, true);
            }
            else if (options[0]._Equal(comboboxes[0].options[1]))
            {
                useSSE(pixelBuffer, 0, HEIGHTW, options, true);
            }
            else
            {
                useAVX(pixelBuffer, 0, HEIGHTW, options, true);
            }
        }
    }
}

void Fractal::draw(HDC hdc, Colour* pixelBuffer, GifWriter* gif, bool recording) 
{
    // Define the bitmap
    BITMAPINFO bmpInfo;
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = WIDTHW;
    bmpInfo.bmiHeader.biHeight = -HEIGHTW; // Negative to indicate top-down bitmap
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 32; // 32 bits per pixel (COLORREF format)
    bmpInfo.bmiHeader.biCompression = BI_RGB;

    // Transfer the pixelBuffer to the screen
    SetDIBitsToDevice(
        hdc,
        0, 0, WIDTHW, HEIGHTW,         // Destination rectangle on the screen
        0, 0, 0, HEIGHTW,             // Source rectangle in the buffer
        pixelBuffer,                 // Pixel buffer source
        &bmpInfo,                    // Bitmap information
        DIB_RGB_COLORS               // Color format (RGB)
    );

    if (recording)
    {
        GifWriteFrame(gif, (uint8_t*)pixelBuffer, WIDTHW, HEIGHTW, GIF_DELAY);
    }
}

void Fractal::zoomScreen(zoomType zoom)
{
    const double xRange = xMax - xMin, yRange = yMax - yMin;
    const double midPx = (xMax + xMin) / 2, midPy = (yMax + yMin) / 2;
    double dx = 0, dy = 0;

    // Zoom in/out according to the zoom factor
    switch (zoom)
    {
    case zoomType::Z_IN:
        // Calculating the change in x and y
        // Based on the zoom factor
        dx = xRange / ZF / 2;
        dy = yRange / ZF / 2;

        break;
    case zoomType::Z_OUT:
        // Update the boundaries of the complex plane
        // Based on the zoom factor
        dx = xRange * ZF / 2;
        dy = yRange * ZF / 2;

        break;
    default:
        break;
    }

    // Updating x/y values with changes
    xMin = midPx - dx;
    xMax = midPx + dx;
    yMin = midPy - dy;
    yMax = midPy + dy;
}

void Fractal::moveScreen(POINT* clickPoint)
{
    // Finding the current ranges of the window
    const double xRange = xMax - xMin, yRange = yMax - yMin;

    // Mapping the window pos to a complex plane pos
    // This will be the new center of the screen
    double xMouse = xMin + (clickPoint->x / static_cast<double>(WIDTHW)) * (xRange);
    double yMouse = yMin + (clickPoint->y / static_cast<double>(HEIGHTW)) * (yRange);

    // Update the boundaries of the complex plane
    // Based on the pos of the mouse click
    xMin = xMouse - xRange / 2.0;
    xMax = xMouse + xRange / 2.0;
    yMin = yMouse - yRange / 2.0;
    yMax = yMouse + yRange / 2.0;
}