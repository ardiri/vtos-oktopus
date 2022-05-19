/*
 * @(#)animate.c
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

// global variable structure
typedef struct
{
  UWORD tmrReference;
} AnimateGlobals;

// globals reference
static AnimateGlobals *globals;

/**
 * Initialize the animation engine.
 */
void 
AnimateInitialize()
{
  // create a globals object
  globals = (AnimateGlobals *)pmalloc(sizeof(AnimateGlobals));
  globals->tmrReference = NULL;
}

/**
 * Start animating..
 *
 * @param fps the number of frames per second required.
 */
void
AnimateStart(WORD fps)
{
  if (!globals->tmrReference)
    globals->tmrReference = TmrIntEnable(1000/fps, AnimateCallback);
}

/**
 * Animation callback ground - to add NULL_EVENT to queue.
 */
void
AnimateCallback()
{
  EvtAppendEvt(NULL_EVENT,0,0,0,NULL);
}

/**
 * Stop animation.
 */
void
AnimateStop()
{
  if (globals->tmrReference)
  {
    TmrIntDisable(globals->tmrReference);
    globals->tmrReference = NULL;
  }
}

/**
 * Terminate the animation engine.
 */
void 
AnimateTerminate()
{
  AnimateStop();

  // clean up memory
  pfree(globals);
}
