/* 
 *  Arnold emulator (c) Copyright, Kevin Thacker 1995-2001
 *  
 *  This file is part of the Arnold emulator source code distribution.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include "yiq.h"
#include <math.h>

#ifndef TRUE
#define TRUE (1==1)
#endif

#ifndef FALSE
#define FALSE (1==0)
#endif

typedef int BOOL;

typedef struct 
{
        float R;
        float G;
        float B;
} RGB_FLOAT;

typedef struct 
{
        float   Y;
        float   I;
        float   Q;
} YIQ_FLOAT;

typedef struct 
{
        float   x;
        float   y;
        float   z;
} VECTOR_FLOAT;

static float RGB2YIQ_NTSC_MATRIX[3][3];
static float YIQ2RGB_NTSC_MATRIX[3][3];

static void    TransformVector_FLOAT(VECTOR_FLOAT *pSrcVector, VECTOR_FLOAT *pDestVector, float (*pMatrix)[3])
{
        VECTOR_FLOAT TempVector;

        TempVector.x = pSrcVector->x;
        TempVector.y = pSrcVector->y;
        TempVector.z = pSrcVector->z;

        pDestVector->x = (pMatrix[0][0] * TempVector.x) +
                                        (pMatrix[0][1] * TempVector.y) +
                                        (pMatrix[0][2] * TempVector.z);

        pDestVector->y = (pMatrix[1][0] * TempVector.x) +
                                        (pMatrix[1][1] * TempVector.y) +
                                        (pMatrix[1][2] * TempVector.z);

        pDestVector->z = (pMatrix[2][0] * TempVector.x) +
                                        (pMatrix[2][1] * TempVector.y) +
                                        (pMatrix[2][2] * TempVector.z);
}

void    BrightnessControl_Initialise(void)
{
        RGB2YIQ_NTSC_MATRIX[0][0] = 0.299f;
        RGB2YIQ_NTSC_MATRIX[0][1] = 0.587f;
        RGB2YIQ_NTSC_MATRIX[0][2] = 0.114f;
        RGB2YIQ_NTSC_MATRIX[1][0] = 0.596f;
        RGB2YIQ_NTSC_MATRIX[1][1] = -0.275f;
        RGB2YIQ_NTSC_MATRIX[1][1] = -0.321f;
        RGB2YIQ_NTSC_MATRIX[2][0] = 0.212f;
        RGB2YIQ_NTSC_MATRIX[2][1] = -0.523f;
        RGB2YIQ_NTSC_MATRIX[2][2] = 0.311f;

		YIQ2RGB_NTSC_MATRIX[0][0] = 0.600f;
		YIQ2RGB_NTSC_MATRIX[0][1] = 1.455f;
		YIQ2RGB_NTSC_MATRIX[0][2] = -0.220f;
		YIQ2RGB_NTSC_MATRIX[1][0] = 1.114f;
		YIQ2RGB_NTSC_MATRIX[1][1] = -0.414f;
		YIQ2RGB_NTSC_MATRIX[1][1] = -0.408f;
		YIQ2RGB_NTSC_MATRIX[2][0] = 1.464f;
		YIQ2RGB_NTSC_MATRIX[2][1] = -1.687f;
		YIQ2RGB_NTSC_MATRIX[2][1] = 2.679f;
}



/* convert R,G,B colour triple to YIQ representation */
static void    RGB2YIQ_FLOAT(RGB_FLOAT *pRGB, YIQ_FLOAT *pYIQ, float (*pRGB2YIQ_MATRIX)[3])
{
        TransformVector_FLOAT((VECTOR_FLOAT *)pRGB,(VECTOR_FLOAT *)pYIQ, pRGB2YIQ_MATRIX);
}


/* convert Y,I,Q colour triple to RGB representation */
static void    YIQ2RGB_FLOAT(RGB_FLOAT *pRGB, YIQ_FLOAT *pYIQ, float (*pYIQ2RGB_MATRIX)[3])
{
        TransformVector_FLOAT((VECTOR_FLOAT *)pYIQ, (VECTOR_FLOAT *)pRGB, pYIQ2RGB_MATRIX);
}


void    BrightnessControl_GenerateGreyScaleFromColour(RGB_CHAR *pSourceRGB, RGB_CHAR *pDestRGB)
{
        RGB_FLOAT SourceRGBFloat;
        YIQ_FLOAT YIQ;

        float RGB_CHAR_TO_FLOAT_FACTOR = 1/255.0f;
        float RGB_FLOAT_TO_CHAR_FACTOR = 255.0f;

        /* convert char RGB to float RGB */
        SourceRGBFloat.R = (float)(pSourceRGB->R)*RGB_CHAR_TO_FLOAT_FACTOR;
        SourceRGBFloat.G = (float)(pSourceRGB->G)*RGB_CHAR_TO_FLOAT_FACTOR;
        SourceRGBFloat.B = (float)(pSourceRGB->B)*RGB_CHAR_TO_FLOAT_FACTOR;

        RGB2YIQ_FLOAT(&SourceRGBFloat, &YIQ, RGB2YIQ_NTSC_MATRIX);


        pDestRGB->R = (char)(YIQ.Y * RGB_FLOAT_TO_CHAR_FACTOR);
        pDestRGB->G = (char)(YIQ.Y * RGB_FLOAT_TO_CHAR_FACTOR);
        pDestRGB->B = (char)(YIQ.Y * RGB_FLOAT_TO_CHAR_FACTOR);
}

void    BrightnessControl_AdjustBrightness(RGB_CHAR *pSourceRGB, RGB_CHAR *pDestRGB, int Brightness)
{
        RGB_FLOAT SourceRGBFloat;

        float RGB_CHAR_TO_FLOAT_FACTOR = 1/255.0f;
        float RGB_FLOAT_TO_CHAR_FACTOR = 255.0f;
        float BrightnessFloat = (float)Brightness/256.0f;

        /* convert char RGB to float RGB */
        SourceRGBFloat.R = (float)(pSourceRGB->R)*RGB_CHAR_TO_FLOAT_FACTOR;
        SourceRGBFloat.G = (float)(pSourceRGB->G)*RGB_CHAR_TO_FLOAT_FACTOR;
        SourceRGBFloat.B = (float)(pSourceRGB->B)*RGB_CHAR_TO_FLOAT_FACTOR;

#ifdef POW
		SourceRGBFloat.R = pow(SourceRGBFloat.R,1/BrightnessFloat);
		SourceRGBFloat.G = pow(SourceRGBFloat.G,1/BrightnessFloat);
		SourceRGBFloat.B = pow(SourceRGBFloat.B, 1/BrightnessFloat);
#else
        SourceRGBFloat.R = SourceRGBFloat.R;
        SourceRGBFloat.G = SourceRGBFloat.G;
        SourceRGBFloat.B = SourceRGBFloat.B;
#endif
        pDestRGB->R = (char)(SourceRGBFloat.R * RGB_FLOAT_TO_CHAR_FACTOR);
        pDestRGB->G = (char)(SourceRGBFloat.G * RGB_FLOAT_TO_CHAR_FACTOR);
        pDestRGB->B = (char)(SourceRGBFloat.B * RGB_FLOAT_TO_CHAR_FACTOR);
}

