/* FFBMP - Fast n' Filthy BMP
 * https://github.com/molkoback/ffbmp
 * 
 * v1.0.1 - 2018-06-01
 * 
 * The library supports the following BMP variants:
 * 1. Uncompressed 32 BPP (alpha values are ignored)
 * 2. Uncompressed 24 BPP
 * 3. Uncompressed 8 BPP (indexed color)
 * 
 * 
 * Copyright (c) 2007 Chai Braudo <braudo@users.sourceforge.net>
 * Copyright (c) 2018 Eero Molkoselkä <eero.molkoselka@gmail.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef _BMP_H_
#define _BMP_H_
#define _FFBMP_H_

#include <stdint.h>

/* Version information. */
#define FFBMP_VERSION_MAJOR 1
#define FFBMP_VERSION_MINOR 0
#define FFBMP_VERSION_PATCH 0

/* Type definitions. */
#define UINT uint32_t
#define USHORT uint16_t
#define UCHAR uint8_t

/* BMP error codes. */
enum {
	BMP_OK = 0,                  // No error
	BMP_OUT_OF_MEMORY = -1,      // Couldn't allocate enough memory
	BMP_IO_ERROR = -2,           // General input/output error
	BMP_FILE_OPEN_ERROR = -3,    // Couldn't open the file
	BMP_FILE_TYPE_ERROR = -4,    // File is not a BMP image or is an invalid BMP
	BMP_FILE_NOT_SUPPORTED = -5, // File is not a supported BMP variant
};

/* BMP header. */
typedef struct BMP_Header {
	USHORT Magic;         // Magic identifier: "BM"
	UINT FileSize;        // Size of the BMP file in bytes
	USHORT Reserved1;     // Reserved
	USHORT Reserved2;     // Reserved
	UINT DataOffset;      // Offset of image data relative to the file's start
	UINT HeaderSize;      // Size of the header in bytes
	UINT Width;           // Bitmap's width
	UINT Height;          // Bitmap's height
	USHORT Planes;        // Number of color planes in the bitmap
	USHORT BitsPerPixel;  // Number of bits per pixel
	UINT CompressionType; // ompression type
	UINT ImageDataSize;   // Size of uncompressed image's data
	UINT HPixelsPerMeter; // Horizontal resolution (pixels per meter)
	UINT VPixelsPerMeter; // Vertical resolution (pixels per meter)
	UINT ColorsUsed;      // Number of color indexes in the color table that are actually used by the bitmap
	UINT ColorsRequired;  // Number of color indexes that are required for displaying the bitmap
} BMP_Header;

/* BMP image. */
typedef struct BMP {
	BMP_Header Header;
	UCHAR *Palette;
	UCHAR *Data;
} BMP;

/* Creates a blank BMP image with the specified dimensions and bit depth. */
int BMP_Create(BMP *bmp, UINT width, UINT height, USHORT depth);

/* Frees all the memory used by the specified BMP image. */
void BMP_Free(BMP *bmp);

/* Reads the specified BMP image file. */
int BMP_ReadFile(BMP *bmp, const char *filename);

/* Writes the BMP image to the specified file. */
int BMP_WriteFile(BMP *bmp, const char *filename);

/* Populates the arguments with the specified pixel's RGB values. */
void BMP_GetPixelRGB(BMP *bmp, UINT x, UINT y, UCHAR *r, UCHAR *g, UCHAR *b);

/* Sets the specified pixel's RGB values. */
void BMP_SetPixelRGB(BMP *bmp, UINT x, UINT y, UCHAR r, UCHAR g, UCHAR b);

/* Gets the specified pixel's color index. */
void BMP_GetPixelIndex(BMP *bmp, UINT x, UINT y, UCHAR *val);

/* Sets the specified pixel's color index. */
void BMP_SetPixelIndex(BMP *bmp, UINT x, UINT y, UCHAR val);

/* Gets the color value for the specified palette index. */
void BMP_GetPaletteColor(BMP *bmp, UCHAR index, UCHAR *r, UCHAR *g, UCHAR *b);

/* Sets the color value for the specified palette index. */
void BMP_SetPaletteColor(BMP *bmp, UCHAR index, UCHAR r, UCHAR g, UCHAR b);

/* Returns a description of the error code. */
const char *BMP_ErrorString(int err);

#endif
