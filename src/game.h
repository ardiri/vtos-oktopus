/*
 * @(#)game.h
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

#ifndef _GAME_H
#define _GAME_H

#include "helio.h"

enum gameType 
{ 
  GAME_A = 0, 
  GAME_B 
};

enum spriteType
{
  spriteDigit = 0,
  spriteLife,
  spriteDiver,
  spriteDiverDeath,
  spriteTentacle
};

enum moveType
{
  moveNone    = 0,
  moveLeft,
  moveRight
};

extern void   GameInitialize();
extern void   GameReset(PreferencesType *, BYTE);
extern void   GameResetPreferences(PreferencesType *, BYTE);
extern void   GameProcessKeyInput(PreferencesType *, UWORD);
extern void   GameProcessStylusInput(PreferencesType *, SHORT, SHORT);
extern void   GameMovement(PreferencesType *);
extern void   GameDraw(PreferencesType *);
extern void   GameTerminate();

#endif 
