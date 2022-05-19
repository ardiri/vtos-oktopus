/*
 * @(#)device.h
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

#ifndef _DEVICE_H
#define _DEVICE_H

#include "helio.h"

// typedefs, constants
#define EVT_FORM_CONFIGURE 1024

#define evtWaitForever    -1
#define noPreferenceFound -1

#define keyBitPower       0x0001  // power button
#define keyBitPageUp      0x0002  
#define keyBitPageDown    0x0004  // scroll buttons [side of helio]
#define keyBitHard1       0x0008
#define keyBitHard2       0x0010
#define keyBitHard3       0x0020  // application buttons

// functions
extern void  DeviceInitialize();
extern void  DeviceGetEvent(EvtType *, SHORT);
extern SHORT DeviceGetTicks();
extern SHORT DeviceTicksPerSecond();
extern void  DeviceGetOSVersionNumber(BYTE *);
extern SHORT DeviceGetAppPreferences(void *, SHORT *, BOOLEAN);
extern void  DeviceSetAppPreferences(void *, SHORT, BOOLEAN);
extern SHORT DeviceRandom(WORD);
extern UWORD DeviceKeyCurrentState();
extern void  DeviceTerminate();

#endif 
