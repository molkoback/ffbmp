/* QDBMP - Quick n' Dirty BMP
 * 
 * v2.0.0 - 2018-05-30
 * 
 * The library supports the following BMP variants:
 * 1. Uncompressed 32 BPP (alpha values are ignored)
 * 2. Uncompressed 24 BPP
 * 3. Uncompressed 8 BPP (indexed color)
 * 
 * 
 * GitHub:     https://github.com/molkoback/qdbmp
 * Maintainer: Eero Molkoselkä <eero.molkoselka@gmail.com>
 * 
 * 
 * Copyright (c) 2007 Chai Braudo <braudo@users.sourceforge.net>
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

#include <stdint.h>

/* Type definitions. */
#define UINT uint32_t
#define USHORT uint16_t
#define UCHAR uint8_t

/* Version information. */
#define QDBMP_VERSION_MAJOR 2
#define QDBMP_VERSION_MINOR 0
#define QDBMP_VERSION_PATCH 0

/* BMP error codes. */
typedef enum {
	BMP_OK = 0,             // No error
	BMP_ERROR,              // General error
	BMP_OUT_OF_MEMORY,      // Could not allocate enough memory to complete the operation
	BMP_IO_ERROR,           // General input/output error
	BMP_FILE_NOT_FOUND,     // File not found
	BMP_FILE_NOT_SUPPORTED, // File is not a supported BMP variant
	BMP_FILE_INVALID,       // File is not a BMP image or is an invalid BMP
	BMP_INVALID_ARGUMENT,   // An argument is invalid or out of range
	BMP_TYPE_MISMATCH,      // The requested action is not compatible with the BMP's type
	BMP_ERROR_NUM
} BMP_STATUS;

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

/* Create and free BMP. */
BMP *BMP_Create(UINT width, UINT height, USHORT depth);
void BMP_Free(BMP *bmp);

/* File access. */
BMP *BMP_ReadFile(const char *filename);
void BMP_WriteFile(BMP *bmp, const char *filename);

/* Pixel access. */
void BMP_GetPixelRGB(BMP *bmp, UINT x, UINT y, UCHAR *r, UCHAR *g, UCHAR *b);
void BMP_SetPixelRGB(BMP *bmp, UINT x, UINT y, UCHAR r, UCHAR g, UCHAR b);
void BMP_GetPixelIndex(BMP *bmp, UINT x, UINT y, UCHAR *val);
void BMP_SetPixelIndex(BMP *bmp, UINT x, UINT y, UCHAR val);

/* Palette handling. */
void BMP_GetPaletteColor(BMP *bmp, UCHAR index, UCHAR *r, UCHAR *g, UCHAR *b);
void BMP_SetPaletteColor(BMP *bmp, UCHAR index, UCHAR r, UCHAR g, UCHAR b);

/* Error handling. */
BMP_STATUS BMP_GetError();
const char *BMP_GetErrorDescription();

#endif
