/*
 * @(#)animate.h
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

#ifndef _ANIMATE_H
#define _ANIMATE_H

#include "helio.h"

extern void   AnimateInitialize();
extern void   AnimateStart(WORD);
extern void   AnimateCallback();
extern void   AnimateStop();
extern void   AnimateTerminate();

#endif 
