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

#include "qdbmp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Size of the palette data for 8 BPP bitmaps. */
#define BMP_PALETTE_SIZE (256 * 4)

/* Holds the last error code. */
static BMP_STATUS last_error = 0;

/* Reads a little-endian unsigned int from the file.
 * Returns non-zero on success. */
int ReadUINT(UINT *x, FILE *fp)
{
	UCHAR little[4];
	
	if (x == NULL || fp == NULL)
		return 0;
	if (fread(little, 4, 1, fp) != 1)
		return 0;
	
	*x = little[3] << 24 | little[2] << 16 | little[1] << 8 | little[0];
	return 1;
}

/* Reads a little-endian unsigned short int from the file.
 * Returns non-zero on success. */
int ReadUSHORT(USHORT *x, FILE *fp)
{
	UCHAR little[2];
	
	if (x == NULL || fp == NULL)
		return 0;
	if (fread(little, 2, 1, fp) != 1)
		return 0;
	
	*x = little[1] << 8 | little[0];
	return 1;
}

/* Reads the BMP file's header into the data structure.
 * Returns BMP_OK on success. */
int ReadHeader(BMP *bmp, FILE *fp)
{
	if (bmp == NULL || fp == NULL)
		return BMP_INVALID_ARGUMENT;
	
	// The header's fields are read one by one, and converted from the format's
	// little endian to the system's native representation.
	if (!ReadUSHORT(&bmp->Header.Magic, fp)) return BMP_IO_ERROR;
	if (!ReadUINT(&bmp->Header.FileSize, fp)) return BMP_IO_ERROR;
	if (!ReadUSHORT(&bmp->Header.Reserved1, fp)) return BMP_IO_ERROR;
	if (!ReadUSHORT(&bmp->Header.Reserved2, fp)) return BMP_IO_ERROR;
	if (!ReadUINT(&bmp->Header.DataOffset, fp)) return BMP_IO_ERROR;
	if (!ReadUINT(&bmp->Header.HeaderSize, fp)) return BMP_IO_ERROR;
	if (!ReadUINT(&bmp->Header.Width, fp)) return BMP_IO_ERROR;
	if (!ReadUINT(&bmp->Header.Height, fp)) return BMP_IO_ERROR;
	if (!ReadUSHORT(&bmp->Header.Planes, fp)) return BMP_IO_ERROR;
	if (!ReadUSHORT(&bmp->Header.BitsPerPixel, fp)) return BMP_IO_ERROR;
	if (!ReadUINT(&bmp->Header.CompressionType, fp)) return BMP_IO_ERROR;
	if (!ReadUINT(&bmp->Header.ImageDataSize, fp)) return BMP_IO_ERROR;
	if (!ReadUINT(&bmp->Header.HPixelsPerMeter, fp)) return BMP_IO_ERROR;
	if (!ReadUINT(&bmp->Header.VPixelsPerMeter, fp)) return BMP_IO_ERROR;
	if (!ReadUINT(&bmp->Header.ColorsUsed, fp)) return BMP_IO_ERROR;
	if (!ReadUINT(&bmp->Header.ColorsRequired, fp)) return BMP_IO_ERROR;
	return BMP_OK;
}

/* Writes a little-endian unsigned int to the file.
 * Returns non-zero on success. */
int WriteUINT(UINT x, FILE *fp)
{
	UCHAR little[4];
	
	little[3] = (UCHAR)((x & 0xff000000) >> 24);
	little[2] = (UCHAR)((x & 0x00ff0000) >> 16);
	little[1] = (UCHAR)((x & 0x0000ff00) >> 8);
	little[0] = (UCHAR)((x & 0x000000ff) >> 0);
	
	return fp != NULL && fwrite(little, 4, 1, fp) == 1;
}

/* Writes a little-endian unsigned short int to the file.
 * Returns non-zero on success. */
int WriteUSHORT(USHORT x, FILE *fp)
{
	UCHAR little[2];
	
	little[1] = (UCHAR)((x & 0xff00) >> 8);
	little[0] = (UCHAR)((x & 0x00ff) >> 0);
	
	return fp != NULL && fwrite(little, 2, 1, fp) == 1;
}

/* Writes the BMP file's header into the data structure.
 * Returns BMP_OK on success. */
int WriteHeader(BMP *bmp, FILE *fp)
{
	if (bmp == NULL || fp == NULL)
		return BMP_INVALID_ARGUMENT;
	 
	// The header's fields are written one by one, and converted to the format's
	// little endian representation.
	if (!WriteUSHORT(bmp->Header.Magic, fp)) return BMP_IO_ERROR;
	if (!WriteUINT(bmp->Header.FileSize, fp)) return BMP_IO_ERROR;
	if (!WriteUSHORT(bmp->Header.Reserved1, fp)) return BMP_IO_ERROR;
	if (!WriteUSHORT(bmp->Header.Reserved2, fp)) return BMP_IO_ERROR;
	if (!WriteUINT(bmp->Header.DataOffset, fp)) return BMP_IO_ERROR;
	if (!WriteUINT(bmp->Header.HeaderSize, fp)) return BMP_IO_ERROR;
	if (!WriteUINT(bmp->Header.Width, fp)) return BMP_IO_ERROR;
	if (!WriteUINT(bmp->Header.Height, fp)) return BMP_IO_ERROR;
	if (!WriteUSHORT(bmp->Header.Planes, fp)) return BMP_IO_ERROR;
	if (!WriteUSHORT(bmp->Header.BitsPerPixel, fp)) return BMP_IO_ERROR;
	if (!WriteUINT(bmp->Header.CompressionType, fp)) return BMP_IO_ERROR;
	if (!WriteUINT(bmp->Header.ImageDataSize, fp)) return BMP_IO_ERROR;
	if (!WriteUINT(bmp->Header.HPixelsPerMeter, fp)) return BMP_IO_ERROR;
	if (!WriteUINT(bmp->Header.VPixelsPerMeter, fp)) return BMP_IO_ERROR;
	if (!WriteUINT(bmp->Header.ColorsUsed, fp)) return BMP_IO_ERROR;
	if (!WriteUINT(bmp->Header.ColorsRequired, fp)) return BMP_IO_ERROR;
	return BMP_OK;
}

/* Creates a blank BMP image with the specified dimensions and bit depth. */
BMP *BMP_Create(UINT width, UINT height, USHORT depth)
{
	BMP *bmp;
	int bytes_per_pixel = depth >> 3;
	UINT bytes_per_row;
	
	if (height == 0 || width == 0) {
		last_error = BMP_INVALID_ARGUMENT;
		return NULL;
	}
	else if (depth != 8 && depth != 24 && depth != 32) {
		last_error = BMP_FILE_NOT_SUPPORTED;
		return NULL;
	}
	
	// Allocate the bitmap data structure
	bmp = calloc(1, sizeof(BMP));
	if (bmp == NULL) {
		last_error = BMP_OUT_OF_MEMORY;
		return NULL;
	}
	
	// Set header default values
	bmp->Header.Magic = 0x4D42;
	bmp->Header.Reserved1 = 0;
	bmp->Header.Reserved2 = 0;
	bmp->Header.HeaderSize = 40;
	bmp->Header.Planes = 1;
	bmp->Header.CompressionType = 0;
	bmp->Header.HPixelsPerMeter = 0;
	bmp->Header.VPixelsPerMeter = 0;
	bmp->Header.ColorsUsed = 0;
	bmp->Header.ColorsRequired = 0;
	
	// Calculate the number of bytes used to store a single image row. This is always
	// rounded up to the next multiple of 4.
	bytes_per_row = width * bytes_per_pixel;
	bytes_per_row += bytes_per_row % 4 ? 4 - bytes_per_row % 4 : 0;
	
	// Set header's image specific values 
	bmp->Header.Width = width;
	bmp->Header.Height = height;
	bmp->Header.BitsPerPixel = depth;
	bmp->Header.ImageDataSize = bytes_per_row * height;
	bmp->Header.FileSize =
		bmp->Header.ImageDataSize + 54 + (depth == 8 ? BMP_PALETTE_SIZE : 0);
	bmp->Header.DataOffset = 54 + (depth == 8 ? BMP_PALETTE_SIZE : 0);
	
	// Allocate palette
	if (bmp->Header.BitsPerPixel == 8) {
		bmp->Palette = (UCHAR*) calloc(BMP_PALETTE_SIZE, sizeof(UCHAR));
		if (bmp->Palette == NULL) {
			last_error = BMP_OUT_OF_MEMORY;
			free(bmp);
			return NULL;
		}
	}
	else {
		bmp->Palette = NULL;
	}
	
	// Allocate pixels
	bmp->Data = (UCHAR*) calloc(bmp->Header.ImageDataSize, sizeof(UCHAR));
	if (bmp->Data == NULL) {
		last_error = BMP_OUT_OF_MEMORY;
		free(bmp->Palette);
		free(bmp);
		return NULL;
	}
	last_error = BMP_OK;
	return bmp;
}

/* Frees all the memory used by the specified BMP image. */
void BMP_Free(BMP *bmp)
{
	if (bmp != NULL) {
		if (bmp->Palette != NULL)
			free(bmp->Palette);
		if (bmp->Data != NULL)
			free(bmp->Data);
		free(bmp);
		last_error = BMP_OK;
	}
}

/* Reads the specified BMP image file. */
BMP *BMP_ReadFile(const char *filename)
{
	BMP *bmp;
	FILE *fp;
	
	if (filename == NULL) {
		last_error = BMP_INVALID_ARGUMENT;
		return NULL;
	}
	
	// Allocate
	bmp = calloc(1, sizeof(BMP));
	if (bmp == NULL) {
		last_error = BMP_OUT_OF_MEMORY;
		return NULL;
	}
	
	// Open file
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		last_error = BMP_FILE_NOT_FOUND;
		free(bmp);
		return NULL;
	}
	
	// Read header
	if (ReadHeader(bmp, fp) != BMP_OK || bmp->Header.Magic != 0x4D42) {
		last_error = BMP_FILE_INVALID;
		fclose(fp);
		free(bmp);
		return NULL;
	}
	
	// Verify that the bitmap variant is supported
	if ((bmp->Header.BitsPerPixel != 32 &&
	     bmp->Header.BitsPerPixel != 24 &&
	     bmp->Header.BitsPerPixel != 8) ||
	    (bmp->Header.CompressionType != 0) ||
	    (bmp->Header.HeaderSize != 40))
	{
		last_error = BMP_FILE_NOT_SUPPORTED;
		fclose(fp);
		free(bmp);
		return NULL;
	}
	
	// Allocate and read palette
	if (bmp->Header.BitsPerPixel == 8) {
		bmp->Palette = (UCHAR*)malloc(BMP_PALETTE_SIZE * sizeof(UCHAR));
		if (bmp->Palette == NULL) {
			last_error = BMP_OUT_OF_MEMORY;
			fclose(fp);
			free(bmp);
			return NULL;
		}
		
		if (fread(bmp->Palette, sizeof(UCHAR), BMP_PALETTE_SIZE, fp) !=
		    BMP_PALETTE_SIZE)
		{
			last_error = BMP_FILE_INVALID;
			fclose(fp);
			free(bmp->Palette);
			free(bmp);
			return NULL;
		}
	}
	// Not an indexed image
	else {
		bmp->Palette = NULL;
	}
	
	/* Allocate memory for image data */
	bmp->Data = (UCHAR*)malloc(bmp->Header.ImageDataSize);
	if (bmp->Data == NULL) {
		last_error = BMP_OUT_OF_MEMORY;
		fclose(fp);
		free(bmp->Palette);
		free(bmp);
		return NULL;
	}
	
	// Read image data
	if (fread( bmp->Data, sizeof(UCHAR), bmp->Header.ImageDataSize, fp) !=
	    bmp->Header.ImageDataSize)
	{
		last_error = BMP_FILE_INVALID;
		fclose(fp);
		free(bmp->Data);
		free(bmp->Palette);
		free(bmp);
		return NULL;
	}
	fclose(fp);
	last_error = BMP_OK;
	return bmp;
}

/* Writes the BMP image to the specified file. */
void BMP_WriteFile(BMP *bmp, const char *filename)
{
	FILE *fp;
	
	if (filename == NULL) {
		last_error = BMP_INVALID_ARGUMENT;
		return;
	}
	
	// Open file
	fp = fopen(filename, "wb");
	if (fp == NULL) {
		last_error = BMP_FILE_NOT_FOUND;
		return;
	}
	
	// Write header
	if (WriteHeader(bmp, fp) != BMP_OK) {
		last_error = BMP_IO_ERROR;
		fclose(fp);
		return;
	}
	
	// Write palette
	if (bmp->Palette) {
		if (fwrite(bmp->Palette, sizeof(UCHAR), BMP_PALETTE_SIZE, fp) !=
		    BMP_PALETTE_SIZE)
		{
			last_error = BMP_IO_ERROR;
			fclose(fp);
			return;
		}
	}
	
	// Write data
	if (fwrite(bmp->Data, sizeof(UCHAR), bmp->Header.ImageDataSize, fp) !=
	    bmp->Header.ImageDataSize)
	{
		last_error = BMP_IO_ERROR;
		fclose(fp);
		return;
	}
	last_error = BMP_OK;
	fclose(fp);
}

/* Populates the arguments with the specified pixel's RGB values. */
void BMP_GetPixelRGB(BMP *bmp, UINT x, UINT y, UCHAR *r, UCHAR *g, UCHAR *b)
{
	UCHAR *pixel;
	UINT bytes_per_row;
	UCHAR bytes_per_pixel;
	
	if (bmp == NULL || x >= bmp->Header.Width || y >= bmp->Header.Height) {
		last_error = BMP_INVALID_ARGUMENT;
	}
	else {
		bytes_per_pixel = bmp->Header.BitsPerPixel >> 3;
		
		// Row's size is rounded up to the next multiple of 4 bytes 
		bytes_per_row = bmp->Header.ImageDataSize / bmp->Header.Height;
		
		// Calculate the location of the relevant pixel (rows are flipped)
		pixel = bmp->Data + (bmp->Header.Height-y-1)*bytes_per_row + x*bytes_per_pixel;
		
		// In indexed color mode the pixel's value is an index within the palette 
		if (bmp->Header.BitsPerPixel == 8)
			pixel = bmp->Palette + *pixel * 4;
		
		if (b != NULL)
			*b = *(pixel + 0);
		if (g != NULL)
			*g = *(pixel + 1);
		if (r != NULL)
			*r = *(pixel + 2);
		last_error = BMP_OK;
	}
}

/* Sets the specified pixel's RGB values. */
void BMP_SetPixelRGB(BMP *bmp, UINT x, UINT y, UCHAR r, UCHAR g, UCHAR b)
{
	UCHAR *pixel;
	UINT bytes_per_row;
	UCHAR bytes_per_pixel;
	
	if (bmp == NULL || x >= bmp->Header.Width || y >= bmp->Header.Height) {
		last_error = BMP_INVALID_ARGUMENT;
	}
	else if (bmp->Header.BitsPerPixel != 24 && bmp->Header.BitsPerPixel != 32) {
		last_error = BMP_TYPE_MISMATCH;
	}
	else {
		bytes_per_pixel = bmp->Header.BitsPerPixel >> 3;
		
		// Row's size is rounded up to the next multiple of 4 bytes
		bytes_per_row = bmp->Header.ImageDataSize / bmp->Header.Height;
		
		// Calculate the location of the relevant pixel (rows are flipped)
		pixel = bmp->Data + (bmp->Header.Height-y-1)*bytes_per_row + x*bytes_per_pixel;
		
		*(pixel + 0) = b;
		*(pixel + 1) = g;
		*(pixel + 2) = r;
		last_error = BMP_OK;
	}
}

/* Gets the specified pixel's color index. */
void BMP_GetPixelIndex(BMP *bmp, UINT x, UINT y, UCHAR *val)
{
	UCHAR *pixel;
	UINT bytes_per_row;
	
	if (bmp == NULL || x >= bmp->Header.Width || y >= bmp->Header.Height) {
		last_error = BMP_INVALID_ARGUMENT;
	}
	else if (bmp->Header.BitsPerPixel != 8) {
		last_error = BMP_TYPE_MISMATCH;
	}
	else {
		// Row's size is rounded up to the next multiple of 4 bytes
		bytes_per_row = bmp->Header.ImageDataSize / bmp->Header.Height;
		
		// Calculate the location of the relevant pixel
		pixel = bmp->Data + (bmp->Header.Height-y-1)*bytes_per_row + x;
		
		if (val != NULL)
			*val = *pixel;
		last_error = BMP_OK;
	}
}

/* Sets the specified pixel's color index. */
void BMP_SetPixelIndex(BMP *bmp, UINT x, UINT y, UCHAR val)
{
	UCHAR *pixel;
	UINT bytes_per_row;
	
	if (bmp == NULL || x >= bmp->Header.Width || y >= bmp->Header.Height) {
		last_error = BMP_INVALID_ARGUMENT;
	}
	else if (bmp->Header.BitsPerPixel != 8) {
		last_error = BMP_TYPE_MISMATCH;
	}
	else {
		// Row's size is rounded up to the next multiple of 4 bytes
		bytes_per_row = bmp->Header.ImageDataSize / bmp->Header.Height;
		
		// Calculate the location of the relevant pixel
		pixel = bmp->Data + (bmp->Header.Height-y-1)*bytes_per_row + x;
		*pixel = val;
		last_error = BMP_OK;
	}
}

/* Gets the color value for the specified palette index. */
void BMP_GetPaletteColor(BMP *bmp, UCHAR index, UCHAR *r, UCHAR *g, UCHAR *b)
{
	if (bmp == NULL) {
		last_error = BMP_INVALID_ARGUMENT;
	}
	else if  (bmp->Header.BitsPerPixel != 8) {
		last_error = BMP_TYPE_MISMATCH;
	}
	else {
		if (b != NULL)
			*b = *(bmp->Palette + index * 4 + 0);
		if (g != NULL)
			*g = *(bmp->Palette + index * 4 + 1);
		if (r != NULL)
			*r = *(bmp->Palette + index * 4 + 2);
		last_error = BMP_OK;
	}
}

/* Sets the color value for the specified palette index. */
void BMP_SetPaletteColor( BMP* bmp, UCHAR index, UCHAR r, UCHAR g, UCHAR b )
{
	if (bmp == NULL) {
		last_error = BMP_INVALID_ARGUMENT;
	}
	else if (bmp->Header.BitsPerPixel != 8) {
		last_error = BMP_TYPE_MISMATCH;
	}
	else {
		*(bmp->Palette + index * 4 + 0) = b;
		*(bmp->Palette + index * 4 + 1) = g;
		*(bmp->Palette + index * 4 + 2) = r;
		last_error = BMP_OK;
	}
}

/* Returns the last error code. */
BMP_STATUS BMP_GetError() { return last_error; }

/* Returns a description of the last error code. */
const char *BMP_GetErrorDescription()
{
	switch (last_error) {
	case BMP_OK: return "BMP_OK";
	case BMP_ERROR: return "BMP_ERROR";
	case BMP_OUT_OF_MEMORY: return "BMP_OUT_OF_MEMORY";
	case BMP_IO_ERROR: return "BMP_IO_ERROR";
	case BMP_FILE_NOT_FOUND: return "BMP_FILE_NOT_FOUND";
	case BMP_FILE_NOT_SUPPORTED: return "BMP_FILE_NOT_SUPPORTED";
	case BMP_FILE_INVALID: return "BMP_FILE_INVALID";
	case BMP_INVALID_ARGUMENT: return "BMP_INVALID_ARGUMENT";
	case BMP_TYPE_MISMATCH: return "BMP_TYPE_MISMATCH";
	case BMP_ERROR_NUM: return "BMP_ERROR_NUM";
	default: return "UNKNOWN";
	}
}
