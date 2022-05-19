/*
 * @(#)help.c
 *
 * Copyright 1999-2001, Aaron Ardiri (mailto:aaron@ardiri.com)
 * All rights reserved.
 *
 * This file was  generated as part of the  "oktopus" program developed 
 * for the Helio Computing Platform designed by vtech:
 *
 *   http://www.vtechinfo.com/
 *
 * The contents of this file is confidential and  proprietary in nature
 * ("Confidential Information"). Redistribution or modification without 
 * prior consent of the original author is prohibited.
 */

#include "helio.h"

// image resources
#include "help.inc"

// global variable structure
typedef struct
{
  GfxWindow *helpWindow;
} HelpGlobals;

// globals reference
static HelpGlobals *globals;

/**
 * Initialize the help instructions screen.
 *
 * @return the height in pixels of the instructions data area.
 */
SHORT
HelpInitialize()
{
  GfxRegion region = {{   0,   0 }, { 136, 261 }};

  // create a globals object
  globals = (HelpGlobals *)pmalloc(sizeof(HelpGlobals));

  // initialize windows
  globals->helpWindow = GfxCreateWindow(region.extent.x, region.extent.y);

  // draw the help
  {
    GfxFont   currFont;
    GfxWindow *currWindow;
    GfxWindow *tmpWindow;

    currWindow = GfxGetDrawWindow();
    currFont   = GfxGetFont();

    // draw to help window
    GfxSetDrawWindow(globals->helpWindow);
    GfxFillRegion(globals->helpWindow, &region, gfx_white);

    GfxSetFont(gfx_palmosNormalFont);
    {
      BYTE  *str, *ptrStr;
      SHORT x, y;

      // initialize
      y   = 2;
      str = (BYTE *)pmalloc(256 * sizeof(BYTE));

      // draw title
      strcpy(str, "HOW TO PLAY");
      x = (region.extent.x - GfxGetCharsWidth(str, strlen(str))) >> 1;
      GfxDrawString(str, strlen(str), x, y, gfxPaint); 
      y += GfxGetFontHeight();
      GfxDrawLine(x,y-1,x+GfxGetCharsWidth(str,strlen(str))-2,y-1,gfx_black);

      // add a space (little)
      y += GfxGetFontHeight() >> 1;

      // general text
      x = 4;
      strcpy(str,
"Help the diver collect as much gold as possible from the shipwreck that \
is protected by the octopus guarding it.");
      ptrStr = str;
      while (strlen(ptrStr) != 0) {
        BYTE count = GfxGetWordWrap(ptrStr, region.extent.x-x);

        x = (region.extent.x - GfxGetCharsWidth(ptrStr, count)) >> 1;
        GfxDrawString(ptrStr, count, x, y, gfxPaint); 
        y += GfxGetFontHeight(); x = 4;

        ptrStr += count;
      }

      // add a space (little)
      y += GfxGetFontHeight() >> 1;

      // show the movement 
      {
        GfxRegion r = {{0,0},{bitmap00Width,bitmap00Height}};

        // initialize
        tmpWindow = (GfxWindow *)pmalloc(sizeof(GfxWindow));
        tmpWindow->width   = bitmap00Width;
        tmpWindow->height  = bitmap00Height;
        tmpWindow->memSize = bitmap00Size;
        tmpWindow->memory  = (void *)bitmap00;

        // copy the bitmap resource
        x = (region.extent.x - bitmap00Width) >> 1;
        GfxCopyRegion(tmpWindow, globals->helpWindow, &r, x, y, gfxPaint);

        // clean up
        pfree(tmpWindow);
      }

      // add a space (little)
      y += 80 + (GfxGetFontHeight() >> 1);

      // general text
      x = 4;
      strcpy(str,
"Using the stylus or keys, move the diver left and right from the boat \
to the treasure chest as shown above. Dont forget to take the gold back \
to the boat.");
      ptrStr = str;
      while (strlen(ptrStr) != 0) {
        BYTE count = GfxGetWordWrap(ptrStr, region.extent.x-x);

        x = (region.extent.x - GfxGetCharsWidth(ptrStr, count)) >> 1;
        GfxDrawString(ptrStr, count, x, y, gfxPaint); 
        y += GfxGetFontHeight(); x = 4;

        ptrStr += count;
      }

      // add a space (little)
      y += GfxGetFontHeight() >> 1;

      x = 4;
      strcpy(str,
"Game A is for beginners, where Game B requires more skill and \
co-ordination to play.");
      ptrStr = str;
      while (strlen(ptrStr) != 0) {
        BYTE count = GfxGetWordWrap(ptrStr, region.extent.x-x);

        x = (region.extent.x - GfxGetCharsWidth(ptrStr, count)) >> 1;
        GfxDrawString(ptrStr, count, x, y, gfxPaint); 
        y += GfxGetFontHeight(); x = 4;

        ptrStr += count;
      }
    
      // add a space (little)
      y += GfxGetFontHeight() >> 1;

      GfxSetFont(gfx_palmosBoldFont);
      x = 4;
      strcpy(str, "GOOD LUCK!");
      x = (region.extent.x - GfxGetCharsWidth(str, strlen(str))) >> 1;
      GfxDrawString(str, strlen(str), x, y, gfxPaint);
      y += GfxGetFontHeight();
      GfxSetFont(gfx_palmosNormalFont);

      // clean up
      pfree(str);
    }

    // restore settings
    GfxSetFont(currFont);
    GfxSetDrawWindow(currWindow);
  }
  
  return region.extent.y;
}

/**
 * Draw the instructions on the screen.
 *
 * @param offset the offset height of the window to start copying from.
 */
void
HelpDrawInstructions(SHORT offset)
{
  GfxRegion helpArea = {{   0, offset }, { 136, 116 }};

  // blit the required area
  GfxCopyRegion(globals->helpWindow, GfxGetDrawWindow(),
                &helpArea, 7, 20, gfxPaint);
}

/**
 * Terminate the help instructions screen.
 */
void
HelpTerminate()
{
  // clean up memory
  GfxDisposeWindow(globals->helpWindow);
  pfree(globals);
}
