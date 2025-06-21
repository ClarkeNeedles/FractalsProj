/*********************************************************************************************
**
**	File Name:		colour.h
**	Description:	This is the header file that contains my personal Colour struct
**
**	Author:			Clarke Needles
**	Created:		5/21/2025
**
**********************************************************************************************/

#pragma once

#include <stdint.h>

// unsigned 8-bit integer because 0-255
// matches the color codes for RGB perfectly
struct Colour
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};