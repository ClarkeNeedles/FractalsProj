/*********************************************************************************************
**
**	File Name:		winsettings.h
**	Description:	This is the header file that contains all the variables needed for the
**					creation of the default window
**
**	Author:			Clarke Needles
**	Created:		5/14/2025
**
**********************************************************************************************/

#pragma once

#include <string>
#include <Windows.h>
#include <vector>
#include "colour.h"

// Window dimensions
#define WIDTHW 900
#define HEIGHTW 600

// Button IDs (in order L -> R based on UI)
#define ID_LANGCB 1001 
#define ID_FRACCB 1002
#define ID_GRADCB 1003
#define ID_GENBUTTON 1004
#define ID_RECBUTTON 1005

// Button sizes
#define WIDTHB WIDTHW/7 // 7 is the number of buttons + text L to R
#define HEIGHTB 25

// GIF parameters
#define GIF_DELAY 10

// Structure for creating a button
struct CBInfo
{
    LPCWSTR label;
    std::vector<std::wstring> options;
    int id, x, y, width, height, items;

    // Label, id, and height must be passed in
    // Height will vary depending on number of options in drop down
    CBInfo(LPCWSTR label, std::wstring options_in[], int id, int items)
        : label(label), id(id), x(2*WIDTHB*(id % 1000 - 1)), y(0),
        width(WIDTHB), height(HEIGHTB + (20 * items)), items(items) {
        for (int i = 0; i < items; ++i)
        {
            options.push_back(options_in[i]);
        }
    }
};

// Combo box options
static std::wstring langOptions[] = {
    TEXT("C++"),
    TEXT("SSE (128-bit reg's)"),
    TEXT("AVX (256-bit reg's)"),
    TEXT("C++ Multithreaded"),
    TEXT("SSE Multithreaded"),
    TEXT("AVX Multithreaded")
};

static std::wstring fracOptions[] = {
    TEXT("Mandelbrot"),
    TEXT("Burning Ship"),
    TEXT("Multibrot (Order 5)"),
    TEXT("Nova"),
    TEXT("Pheonix")
};

static std::wstring gradOptions[] = {
    TEXT("1"),
    TEXT("2"),
    TEXT("3"),
    TEXT("4"),
    TEXT("5"),
    TEXT("6"),
    TEXT("7")
};

// Combo box declaration
static CBInfo comboboxes[] = {
    { L"Language", langOptions, ID_LANGCB, 6 },
    { L"Fractal", fracOptions, ID_FRACCB, 5 },
    { L"Gradient", gradOptions, ID_GRADCB, 7 },
};