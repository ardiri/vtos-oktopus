/*
 * @(#)gfx.h
 *
 * Copyright 2000, Aaron Ardiri (mailto:aaron@ardiri.com)
 * All rights reserved.
 *
 * This file  was generated as part  of the "gfx" graphics library that
 * is specifically  developed for the Helio Computing Platform designed 
 * by vtech:
 *
 *   http://www.vtechinfo.com/
 *
 * The contents of this file is confidential and  proprietary in nature
 * ("Confidential Information"). Redistribution or modification without 
 * prior consent of the original author is prohibited.
 */

#ifndef _GRAFIX_H
#define _GRAFIX_H

// system includes
#include <system.h>

// typedefs, constants
#define gfx_beta     0x0000
#define gfx_version1 0x0100

typedef enum
{
  gfx_white = 0,     // 0000b
  gfx_lgray = 5,     // 0101b
  gfx_dgray = 10,    // 1010b
  gfx_black = 15     // 1111b
} GfxColor;

typedef enum
{
  gfxPaint = 0,      // x = y
  gfxMask,           // x = x & ~y
  gfxInvert,         // x = x ^ y
  gfxOverlay         // x = x | y
} GfxDrawOperation;

#define gfx_maxFonts      128      // 128 fonts, MAX!
#define gfx_fontCharCount 0x80
#define gfx_fontCharMask  0x7F

typedef enum
{
  gfx_helioSmallFont       = 0,    // vt-os
  gfx_palmosNormalFont     = 16,   // palmos
  gfx_palmosBoldFont,
  gfx_firstUserDefinedFont = 32    // user defined?
} GfxFont;

typedef struct
{
  SHORT width;
  SHORT height;
  WORD  memSize;
  void  *memory;
} GfxWindow;

typedef struct
{
  SHORT x;
  SHORT y;
} GfxPosition;

typedef struct
{
  GfxPosition topLeft;
  GfxPosition extent;
} GfxRegion;

// functions
extern void       GfxInitialize();
extern SHORT      GfxGetVersion();
extern GfxWindow* GfxCreateWindow(SHORT, SHORT);
extern void       GfxDisposeWindow(GfxWindow *);
extern GfxWindow* GfxGetDisplayWindow();
extern void       GfxSetDrawWindow(GfxWindow *);
extern GfxWindow* GfxGetDrawWindow();
extern void       GfxSetFont(GfxFont);
extern void       GfxDefineFont(GfxFont,SHORT,BYTE*,SHORT,SHORT,SHORT*);
extern GfxFont    GfxGetFont();
extern void       GfxClearWindow(GfxWindow *);
extern void       GfxCopyRegion(GfxWindow *, GfxWindow *, 
                                GfxRegion *, SHORT, SHORT, GfxDrawOperation);
extern void       GfxFillRegion(GfxWindow *, GfxRegion *, GfxColor);
extern void       GfxSetPixel(SHORT, SHORT, GfxColor);
extern GfxColor   GfxGetPixel(SHORT, SHORT);
extern void       GfxDrawLine(SHORT, SHORT, SHORT, SHORT, GfxColor);
extern void       GfxDrawString(BYTE *, SHORT, SHORT, SHORT, GfxDrawOperation);
extern SHORT      GfxGetWordWrap(BYTE *, SHORT);
extern SHORT      GfxGetFontHeight();
extern SHORT      GfxGetCharsWidth(BYTE *, SHORT);
extern SHORT      GfxGetCharWidth(BYTE);
extern void       GfxResetClip();
extern void       GfxGetClip(GfxRegion *); 
extern void       GfxSetClip(GfxRegion *); 
extern void       GfxTerminate();

#endif 
