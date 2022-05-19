/*
 * @(#)game.c
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
#include "game.inc"

// interface
static void GameGetSpritePosition(BYTE, BYTE, SHORT *, SHORT *);
static void GameAdjustLevel(PreferencesType *);
static void GameIncrementScore(PreferencesType *);
static void GameMovePlayer(PreferencesType *);
static void GameMoveTentacles(PreferencesType *);

// global variable structure
typedef struct
{
  GfxWindow *winDigits;                     // scoring digits bitmaps
  GfxWindow *winLives;                      // the lives notification bitmaps

  GfxWindow *winDivers;                     // the divers bitmaps
  BOOLEAN   diverChanged;                   // do we need to repaint diver?
  BOOLEAN   diverOnScreen;                  // is diver bitmap on screen?
  USHORT    diverOldPosition;               // the *old* position of diver 

  GfxWindow *winDiverDeaths;                // the diver death bitmaps

  GfxWindow *winTentacles;                  // the tentacle bitmaps
  BOOLEAN   tentacleChanged[MAX_TENT];      // do we need to repaint tentacle
  BOOLEAN   tentacleOnScreen[MAX_TENT];     // is tentacle bitmap on screen?
  USHORT    tentacleOnScreenPosition[MAX_TENT];
                                            // the *old* position of tentacle 

  BYTE      gameType;                       // the type of game active
  BOOLEAN   playerDied;                     // has the player died?
  BYTE      moveDelayCount;                 // the delay between moves
  BYTE      moveLast;                       // the last move performed
  BYTE      moveNext;                       // the next desired move  
} GameGlobals;

// globals reference
static GameGlobals *gbls;

/**
 * Initialize the Game.
 */  
void   
GameInitialize()
{
  // create the globals object
  gbls = (GameGlobals *)pmalloc(sizeof(GameGlobals));

  // initialize and load the "bitmap" windows
  {
    SHORT i;

    gbls->winDigits                    = (GfxWindow *)pmalloc(sizeof(GfxWindow));
    gbls->winDigits->width             = bitmap00Width;
    gbls->winDigits->height            = bitmap00Height;
    gbls->winDigits->memSize           = bitmap00Size;
    gbls->winDigits->memory            = (void *)bitmap00;
    gbls->winLives                     = (GfxWindow *)pmalloc(sizeof(GfxWindow));
    gbls->winLives->width              = bitmap01Width;
    gbls->winLives->height             = bitmap01Height;
    gbls->winLives->memSize            = bitmap01Size;
    gbls->winLives->memory             = (void *)bitmap01;
    gbls->winDivers                    = (GfxWindow *)pmalloc(sizeof(GfxWindow));
    gbls->winDivers->width             = bitmap02Width;
    gbls->winDivers->height            = bitmap02Height;
    gbls->winDivers->memSize           = bitmap02Size;
    gbls->winDivers->memory            = (void *)bitmap02;
    gbls->winDiverDeaths               = (GfxWindow *)pmalloc(sizeof(GfxWindow));
    gbls->winDiverDeaths->width        = bitmap03Width;
    gbls->winDiverDeaths->height       = bitmap03Height;
    gbls->winDiverDeaths->memSize      = bitmap03Size;
    gbls->winDiverDeaths->memory       = (void *)bitmap03;
    gbls->winTentacles                 = (GfxWindow *)pmalloc(sizeof(GfxWindow));
    gbls->winTentacles->width          = bitmap04Width;
    gbls->winTentacles->height         = bitmap04Height;
    gbls->winTentacles->memSize        = bitmap04Size;
    gbls->winTentacles->memory         = (void *)bitmap04;

    gbls->diverChanged                 = TRUE;
    gbls->diverOnScreen                = FALSE;
    gbls->diverOldPosition             = 0;
    
    for (i=0; i<MAX_TENT; i++) {
      gbls->tentacleChanged[i]          = TRUE;
      gbls->tentacleOnScreen[i]         = FALSE;
      gbls->tentacleOnScreenPosition[i] = 0;
    }
  }
}

/**
 * Reset the Game.
 * 
 * @param prefs the global preference data.
 * @param gameType the type of game to configure for.
 */  
void   
GameReset(PreferencesType *prefs, BYTE gameType)
{
  // turn off all the "bitmaps"
  FormDrawForm(gameForm);

  // turn on all the "bitmaps"
  {
    GfxRegion region    = { {   0,   0 }, {   0,   0 } };
    GfxRegion scrRegion = { {   0,   0 }, {   0,   0 } };
    SHORT     index, i, j;

    //
    // draw the score
    //

    for (i=0; i<4; i++) {

      // what is the rectangle we need to copy?
      GameGetSpritePosition(spriteDigit, i,
                            &scrRegion.topLeft.x, &scrRegion.topLeft.y);
      scrRegion.extent.x  = 7;
      scrRegion.extent.y  = 12;
      region.topLeft.x    = 8 * scrRegion.extent.x;
      region.topLeft.y    = 0;
      region.extent.x     = scrRegion.extent.x;
      region.extent.y     = scrRegion.extent.y;

      // draw the digit!
      GfxCopyRegion(gbls->winDigits, GfxGetDrawWindow(),
                    &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxPaint);
    }

    //
    // draw the lives
    //

    // what is the rectangle we need to copy?
    GameGetSpritePosition(spriteLife, 0,
                          &scrRegion.topLeft.x, &scrRegion.topLeft.y);
    scrRegion.extent.x  = 24;
    scrRegion.extent.y  = 14;
    region.topLeft.x    = 0;
    region.topLeft.y    = 0;
    region.extent.x     = scrRegion.extent.x;
    region.extent.y     = scrRegion.extent.y;

    // draw the life bitmap!
    GfxCopyRegion(gbls->winLives, GfxGetDrawWindow(),
                  &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);

    //
    // draw the tentacles
    //

    for (j=0; j<6; j++) {

      BYTE offset[] = { 3, 4, 5, 4, 3, 1 };

      for (i=0; i<offset[j]; i++) {

        index = i + (j * 5);

        // what is the rectangle we need to copy?
        GameGetSpritePosition(spriteTentacle, index, 
                              &scrRegion.topLeft.x, &scrRegion.topLeft.y);
        scrRegion.extent.x  = 15;
        scrRegion.extent.y  = 10;
        region.topLeft.x    = i * scrRegion.extent.x; 
        region.topLeft.y    = j * scrRegion.extent.y; 
        region.extent.x     = scrRegion.extent.x;
        region.extent.y     = scrRegion.extent.y;

        // draw the tentacle bitmap!
        GfxCopyRegion(gbls->winTentacles, GfxGetDrawWindow(),
                      &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);
      }
    }
    
    // 
    // draw the diver death
    //

    for (i=0; i<2; i++) {

      // what is the rectangle we need to copy?
      GameGetSpritePosition(spriteDiverDeath, i, 
                            &scrRegion.topLeft.x, &scrRegion.topLeft.y);
      scrRegion.extent.x  = 29;
      scrRegion.extent.y  = 29;
      region.topLeft.x    = i * scrRegion.extent.x; 
      region.topLeft.y    = 0;
      region.extent.x     = scrRegion.extent.x;
      region.extent.y     = scrRegion.extent.y;

      // draw the diver death bitmap!
      GfxCopyRegion(gbls->winDiverDeaths, GfxGetDrawWindow(),
                    &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);
    }

    //
    // draw the divers
    //

    for (i=0; i<6; i++) {

      // what is the rectangle we need to copy?
      GameGetSpritePosition(spriteDiver, i,
                            &scrRegion.topLeft.x, &scrRegion.topLeft.y);
      scrRegion.extent.x  = 26;
      scrRegion.extent.y  = 26;
      region.topLeft.x    = i * scrRegion.extent.x;
      region.topLeft.y    = 0;
      region.extent.x     = scrRegion.extent.x;
      region.extent.y     = scrRegion.extent.y;

      // draw the diver bitmap!
      GfxCopyRegion(gbls->winDivers, GfxGetDrawWindow(),
                    &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);
    }

    for (i=0; i<2; i++) {

      // what is the rectangle we need to copy?
      GameGetSpritePosition(spriteDiver, 7 + i,
                            &scrRegion.topLeft.x, &scrRegion.topLeft.y);
      scrRegion.extent.x  = 26;
      scrRegion.extent.y  = 26;
      region.topLeft.x    = i * scrRegion.extent.x;
      region.topLeft.y    = 1 * scrRegion.extent.y;
      region.extent.x     = scrRegion.extent.x;
      region.extent.y     = scrRegion.extent.y;

      // draw the diver bitmap!
      GfxCopyRegion(gbls->winDivers, GfxGetDrawWindow(),
                    &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);
    }

    // what is the rectangle we need to copy?
    GameGetSpritePosition(spriteDiver, 0,
                          &scrRegion.topLeft.x, &scrRegion.topLeft.y);
    scrRegion.extent.x  = 26;
    scrRegion.extent.y  = 26;
    region.topLeft.x    = 0;
    region.topLeft.y    = 2 * scrRegion.extent.y;
    region.extent.x     = scrRegion.extent.x;
    region.extent.y     = scrRegion.extent.y;

    // draw the diver bitmap!
    GfxCopyRegion(gbls->winDivers, GfxGetDrawWindow(),
                  &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);
  }

  // wait a good two seconds :))
  TmrWaitTime(2000);

  // turn off all the "bitmaps"
  FormDrawForm(gameForm);

  // reset the preferences
  GameResetPreferences(prefs, gameType);
}

/**
 * Reset the Game preferences.
 * 
 * @param prefs the global preference data.
 * @param gameType the type of game to configure for.
 */  
void   
GameResetPreferences(PreferencesType *prefs, BYTE gameType)
{
  SHORT i, j;

  // now we are playing
  prefs->game.gamePlaying                   = TRUE;
  prefs->game.gamePaused                    = FALSE;
  prefs->game.gameWait                      = TRUE;
  prefs->game.gameAnimationCount            = 0;

  // reset score and lives
  prefs->game.gameScore                     = 0;
  prefs->game.gameLives                     = 3;

  // reset oktopus specific things
  prefs->game.oktopus.gameType              = gameType;
  prefs->game.oktopus.gameLevel             = 1;
  prefs->game.oktopus.bonusAvailable        = TRUE;
  prefs->game.oktopus.bonusScoring          = FALSE;

  prefs->game.oktopus.goldInBag             = 0;

  prefs->game.oktopus.diverCount            = 0;
  prefs->game.oktopus.diverPosition         = 0;
  prefs->game.oktopus.diverNewPosition      = 0;

  memset(&prefs->game.oktopus.tentacleWait[0], (UBYTE)0, sizeof(USHORT) * MAX_TENT);
  memset(&prefs->game.oktopus.tentaclePosition[0], (UBYTE)0, sizeof(USHORT) * MAX_TENT);
  memset(&prefs->game.oktopus.tentacleDirection[0], (UBYTE)0, sizeof(USHORT) * MAX_TENT);

  // reset the "backup" and "onscreen" flags
  gbls->diverChanged                        = TRUE;
  for (i=0; i<MAX_TENT; i++) {
    gbls->tentacleChanged[i]                = TRUE;
    gbls->tentacleOnScreen[i]               = FALSE;
  }

  gbls->gameType                            = gameType;
  gbls->playerDied                          = FALSE;
  gbls->moveDelayCount                      = 0;
  gbls->moveLast                            = moveNone;
  gbls->moveNext                            = moveNone;
}

/**
 * Process key input from the user.
 * 
 * @param prefs the global preference data.
 * @param keyStatus the current key state.
 */  
void   
GameProcessKeyInput(PreferencesType *prefs, UWORD keyStatus)
{
  // the helio device does not have a very nice "key" pattern so
  // playing games using the keys may not be an easy task :) the
  // following is coded as a "prototype", maybe someone will use
  // the "key" capabilities. :))
  //
  // the system is hardcoded as follows:
  //
  //   address book | pageUp    = move left
  //   to do list   | pageDown  = move right
  //
  // :P enjoy
  //
  // -- Aaron Ardiri, 2000

#define ctlKeyLeft  (keyBitHard1 | keyBitPageUp)
#define ctlKeyRight (keyBitHard2 | keyBitPageDown)

  keyStatus &= (ctlKeyLeft  |
                ctlKeyRight);

  // did they press at least one of the game keys?
  if (keyStatus != 0) {

    // if they were waiting, we should reset the game animation count
    if (prefs->game.gameWait) { 
      prefs->game.gameAnimationCount = 0;
      prefs->game.gameWait           = FALSE;
    }

    // great! they wanna play
    prefs->game.gamePaused = FALSE;
  }

  // move left
  if (
      ((keyStatus & ctlKeyLeft) != 0) &&
      (
       (gbls->moveDelayCount == 0) || 
       (gbls->moveLast       != moveLeft)
      )
     ) {

    // adjust the position if possible
    if (prefs->game.oktopus.diverPosition > 0) {
      prefs->game.oktopus.diverNewPosition = 
        prefs->game.oktopus.diverPosition - 1;
    }
  }

  // move right
  else
  if (
      ((keyStatus & ctlKeyRight) != 0) &&
      (
       (gbls->moveDelayCount == 0) || 
       (gbls->moveLast       != moveRight)
      )
     ) {

    // adjust the position if possible
    if (prefs->game.oktopus.diverPosition < 6) {
      prefs->game.oktopus.diverNewPosition = 
        prefs->game.oktopus.diverPosition + 1;
    }
  }
}

/**
 * Process stylus input from the user.
 * 
 * @param prefs the global preference data.
 * @param x the x co-ordinate of the stylus event.
 * @param y the y co-ordinate of the stylus event.
 */  
void   
GameProcessStylusInput(PreferencesType *prefs, SHORT x, SHORT y)
{
  GfxRegion region;
  SHORT     i;

  // lets take a look at all the possible "positions"
  for (i=0; i<7; i++) {

    // get the bounding box of the position
    GameGetSpritePosition(spriteDiver, i,
                          &region.topLeft.x, &region.topLeft.y);
    region.extent.x  = 26;
    region.extent.y  = 26;

    // did they tap inside this rectangle?
    if (((x >= region.topLeft.x) && (y >= region.topLeft.y) && 
         (x <= (region.topLeft.x+region.extent.x)) &&
         (y <= (region.topLeft.y+region.extent.y)))) {

      // ok, this is where we are going to go :)
      prefs->game.oktopus.diverNewPosition = i;

      // if they were waiting, we should reset the game animation count
      if (prefs->game.gameWait) { 
        prefs->game.gameAnimationCount = 0;
        prefs->game.gameWait           = FALSE;
      }

      // great! they wanna play
      prefs->game.gamePaused = FALSE;
      break;                                        // stop looking
    }
  }        
}

/**
 * Process the object movement in the game.
 * 
 * @param prefs the global preference data.
 */  
void   
GameMovement(PreferencesType *prefs)
{
  SHORT     i, j;
  GfxRegion region  = {{   8,  18 }, { 144, 14 }};

  //
  // the game is NOT paused.
  //

  if (!prefs->game.gamePaused) {

    // we must make sure the user is ready for playing 
    if (!prefs->game.gameWait) {

      // we cannot be dead yet :)
      gbls->playerDied = FALSE;

      // are we in bonus mode?
      if ((prefs->game.oktopus.bonusScoring) &&
          (prefs->game.gameAnimationCount % GAME_FPS) < (GAME_FPS >> 1)) {

        BYTE    str[32];
        GfxFont currFont = GfxGetFont();

        strcpy(str, "    * BONUS PLAY *    ");
        GfxSetFont(gfx_palmosBoldFont);
        GfxDrawString(str, strlen(str), 
                      80 - (GfxGetCharsWidth(str, strlen(str)) >> 1), 22, gfxPaint);
        GfxSetFont(currFont);
      }
      else 
        GfxFillRegion(GfxGetDrawWindow(), &region, gfx_white);

      // player gets first move
      GameMovePlayer(prefs);
      GameMoveTentacles(prefs);

      // is it time to upgrade the game?
      if (prefs->game.gameAnimationCount >= 
           ((gbls->gameType == GAME_A) ? 0x17f : 0x100)) {

        prefs->game.gameAnimationCount = 0;
        prefs->game.oktopus.gameLevel++;

        // upgrading of difficulty?
        if (
            (gbls->gameType                == GAME_A) &&
            (prefs->game.oktopus.gameLevel > 12)
           ) {

          gbls->gameType                 = GAME_B;
          prefs->game.oktopus.gameLevel -= 2; // give em a break :)
        }
      } 

      // has the player died in this frame?
      if (gbls->playerDied) {

        SHORT     index;
        GfxRegion region    = { {   0,   0 }, {   0,   0 } };
        GfxRegion scrRegion = { {   0,   0 }, {   0,   0 } };

        // play death sound
        SndPlaySndEffect(SNDRES2_BEEP);
        TmrWaitTime(500);

        // we need to make some changes (to the oktopus)
        prefs->game.oktopus.tentaclePosition[2] = 2;
        gbls->tentacleChanged[2] = TRUE;
        prefs->game.oktopus.tentaclePosition[3] = 2;
        gbls->tentacleChanged[3] = TRUE;
        GameDraw(prefs);

        if (gbls->diverOnScreen) {

          index = gbls->diverOldPosition;

          // what is the rectangle we need to copy?
          GameGetSpritePosition(spriteDiver, index, 
                                &scrRegion.topLeft.x, &scrRegion.topLeft.y);
          scrRegion.extent.x  = 26;
          scrRegion.extent.y  = 26;
          region.topLeft.x    = index * scrRegion.extent.x; 
          region.topLeft.y    = 0;
          region.extent.x     = scrRegion.extent.x;
          region.extent.y     = scrRegion.extent.y;

          // invert the old diver bitmap!
          GfxCopyRegion(gbls->winDivers, GfxGetDrawWindow(),
                        &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxMask);
          gbls->diverOnScreen = FALSE;
        }

        // what is the rectangle we need to copy?
        GameGetSpritePosition(spriteTentacle, 25, 
                              &scrRegion.topLeft.x, &scrRegion.topLeft.y);
        scrRegion.extent.x  = 15;
        scrRegion.extent.y  = 10;
        region.topLeft.x    = 0;
        region.topLeft.y    = 5 * scrRegion.extent.x; 
        region.extent.x     = scrRegion.extent.x;
        region.extent.y     = scrRegion.extent.y;

        // draw the tentacle bitmap!
        GfxCopyRegion(gbls->winTentacles, GfxGetDrawWindow(),
                      &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxInvert);

        // play death sound and flash the player
        for (i=0; i<4; i++) {

          index = i % 2;

          // what is the rectangle we need to copy?
          GameGetSpritePosition(spriteDiverDeath, index, 
                                &scrRegion.topLeft.x, &scrRegion.topLeft.y);
          scrRegion.extent.x  = 29;
          scrRegion.extent.y  = 29;
          region.topLeft.x    = index * scrRegion.extent.x; 
          region.topLeft.y    = 0;
          region.extent.x     = scrRegion.extent.x;
          region.extent.y     = scrRegion.extent.y;

          // draw the diver death bitmap
          GfxCopyRegion(gbls->winDiverDeaths, GfxGetDrawWindow(),
                        &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);

          // play death sound
          SndPlaySndEffect(SNDRES2_BEEP);
          TmrWaitTime(500);

          // invert the diver death bitmap
          GfxCopyRegion(gbls->winDiverDeaths, GfxGetDrawWindow(),
                        &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxMask);
        }

        // what is the rectangle we need to copy?
        GameGetSpritePosition(spriteTentacle, 25, 
                              &scrRegion.topLeft.x, &scrRegion.topLeft.y);
        scrRegion.extent.x  = 15;
        scrRegion.extent.y  = 10;
        region.topLeft.x    = 0;
        region.topLeft.y    = 5 * scrRegion.extent.x; 
        region.extent.x     = scrRegion.extent.x;
        region.extent.y     = scrRegion.extent.y;

        // draw the tentacle bitmap!
        GfxCopyRegion(gbls->winTentacles, GfxGetDrawWindow(),
                      &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxInvert);

        // lose a life :(
        prefs->game.gameLives--;
        {
          index = 0;  // all the lives shown

          // what is the rectangle we need to copy?
          GameGetSpritePosition(spriteLife, 0, 
                                &scrRegion.topLeft.x, &scrRegion.topLeft.y);
          scrRegion.extent.x  = 24;
          scrRegion.extent.y  = 14;
          region.topLeft.x    = index * scrRegion.extent.x;
          region.topLeft.y    = 0;
          region.extent.x     = scrRegion.extent.x;
          region.extent.y     = scrRegion.extent.y;

          // invert the three miss bitmap!
          GfxCopyRegion(gbls->winLives, GfxGetDrawWindow(),
                        &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxMask);
        }

        // no more lives left: GAME OVER!
        if (prefs->game.gameLives == 0) {

          // return to main screen
          EvtAppendEvt(EVT_INLAY_SELECT,0,INLAY_EXIT,0,NULL);

          prefs->game.gamePlaying = FALSE;
        }

        // reset player position and continue game
        else {
          GameAdjustLevel(prefs);
          prefs->game.oktopus.bonusScoring = FALSE;
          prefs->game.gameWait             = TRUE;
        }
      }
    }

    // we have to display "GET READY!"
    else {

      // flash on:
      if ((prefs->game.gameAnimationCount % GAME_FPS) < (GAME_FPS >> 1)) {

        BYTE    str[32];
        GfxFont currFont = GfxGetFont();

        strcpy(str, "    * GET READY *    ");
        GfxSetFont(gfx_palmosBoldFont);
        GfxDrawString(str, strlen(str), 
                      80 - (GfxGetCharsWidth(str, strlen(str)) >> 1), 22, gfxPaint);
        GfxSetFont(currFont);
      }

      // flash off:
      else
        GfxFillRegion(GfxGetDrawWindow(), &region, gfx_white);
    }

    // update the animation counter
    prefs->game.gameAnimationCount++;
  }

  //
  // the game is paused.
  //

  else {

    BYTE    str[32];
    GfxFont currFont = GfxGetFont();

    strcpy(str, "    *  PAUSED  *    ");
    GfxSetFont(gfx_palmosBoldFont);
    GfxDrawString(str, strlen(str), 
                  80 - (GfxGetCharsWidth(str, strlen(str)) >> 1), 22, gfxPaint);
    GfxSetFont(currFont);
  }
}

/**
 * Draw the game on the screen.
 * 
 * @param prefs the global preference data.
 */
void   
GameDraw(PreferencesType *prefs)
{
  SHORT     i, j, index;
  GfxRegion region    = { {   0,   0 }, {   0,   0 } };
  GfxRegion scrRegion = { {   0,   0 }, {   0,   0 } };

  // 
  // DRAW INFORMATION/BITMAPS ON SCREEN
  //

  // draw the score
  {
    short base;
 
    base = 1000;  // max score (4 digits)
    for (i=0; i<4; i++) {

      index = (prefs->game.gameScore / base) % 10;

      // what is the rectangle we need to copy?
      GameGetSpritePosition(spriteDigit, i, 
                            &scrRegion.topLeft.x, &scrRegion.topLeft.y);
      scrRegion.extent.x  = 7;
      scrRegion.extent.y  = 12;
      region.topLeft.x    = index * scrRegion.extent.x;
      region.topLeft.y    = 0;
      region.extent.x     = scrRegion.extent.x;
      region.extent.y     = scrRegion.extent.y;

      // draw the digit!
      GfxCopyRegion(gbls->winDigits, GfxGetDrawWindow(),
                    &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxPaint);
      base /= 10;
    }
  }

  // draw the life that have occurred :( 
  if (prefs->game.gameLives > 1) {
  
    index = 3 - prefs->game.gameLives;

    // what is the rectangle we need to copy?
    GameGetSpritePosition(spriteLife, 0, 
                          &scrRegion.topLeft.x, &scrRegion.topLeft.y);
    scrRegion.extent.x  = 24;
    scrRegion.extent.y  = 14;
    region.topLeft.x    = index * scrRegion.extent.x;
    region.topLeft.y    = 0;
    region.extent.x     = scrRegion.extent.x;
    region.extent.y     = scrRegion.extent.y;

    // draw the life bitmap!
    GfxCopyRegion(gbls->winLives, GfxGetDrawWindow(),
                  &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);
  }

  // no lives left, make sure none are shown
  else {
  
    index = 0;  // all the lives shown

    // what is the rectangle we need to copy?
    GameGetSpritePosition(spriteLife, 0, 
                          &scrRegion.topLeft.x, &scrRegion.topLeft.y);
    scrRegion.extent.x  = 24;
    scrRegion.extent.y  = 14;
    region.topLeft.x    = index * scrRegion.extent.x;
    region.topLeft.y    = 0;
    region.extent.x     = scrRegion.extent.x;
    region.extent.y     = scrRegion.extent.y;

    // invert the three miss bitmap!
    GfxCopyRegion(gbls->winLives, GfxGetDrawWindow(),
                  &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxMask);
  }

  // draw the tentacles
  for (i=0; i<5; i++) {

    // draw the tentacles on the screen (only if it has changed)
    if (gbls->tentacleChanged[i]) {

      //
      // erase the previous tentacle (if any)
      //

      if (gbls->tentacleOnScreen[i]) {

        index = (i * 5);

        for (j=0; j<gbls->tentacleOnScreenPosition[i]; j++) {

          // what is the rectangle we need to copy?
          GameGetSpritePosition(spriteTentacle, index,
                                &scrRegion.topLeft.x, &scrRegion.topLeft.y);
          scrRegion.extent.x  = 15;
          scrRegion.extent.y  = 10;
          region.topLeft.x    = (index % 5) * scrRegion.extent.x;
          region.topLeft.y    = i * scrRegion.extent.y;
          region.extent.x     = scrRegion.extent.x;
          region.extent.y     = scrRegion.extent.y;

          // invert the old tentacle bitmap!
          GfxCopyRegion(gbls->winTentacles, GfxGetDrawWindow(),
                        &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxMask);
          index++;
        }

        gbls->tentacleOnScreen[i] = FALSE;
      }

      //
      // draw the tentacle
      //

      if (prefs->game.oktopus.tentaclePosition[i] > 0) {

        index = (i * 5);

        for (j=0; j<prefs->game.oktopus.tentaclePosition[i]; j++) {

          // what is the rectangle we need to copy?
          GameGetSpritePosition(spriteTentacle, index,
                                &scrRegion.topLeft.x, &scrRegion.topLeft.y);
          scrRegion.extent.x  = 15;
          scrRegion.extent.y  = 10;
          region.topLeft.x    = (index % 5) * scrRegion.extent.x;
          region.topLeft.y    = i * scrRegion.extent.y;
          region.extent.x     = scrRegion.extent.x;
          region.extent.y     = scrRegion.extent.y;

          // draw the new tentacle bitmap!
          GfxCopyRegion(gbls->winTentacles, GfxGetDrawWindow(),
                        &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);
          index++;
        }

        // save this location, record tentacle is onscreen
        gbls->tentacleOnScreen[i]         = TRUE;
        gbls->tentacleOnScreenPosition[i] = prefs->game.oktopus.tentaclePosition[i];
      }

      // dont draw until we need to
      gbls->tentacleChanged[i] = FALSE;
    }
  }

  // draw diver (only if it has changed)
  if (gbls->diverChanged) {

    // 
    // erase the previous diver
    // 

    if (gbls->diverOnScreen) {

      index = gbls->diverOldPosition;

      // what is the rectangle we need to copy?
      GameGetSpritePosition(spriteDiver, index, 
                            &scrRegion.topLeft.x, &scrRegion.topLeft.y);
      scrRegion.extent.x  = 26;
      scrRegion.extent.y  = 26;
      region.topLeft.x    = index * scrRegion.extent.x; 
      region.topLeft.y    = 0;
      region.extent.x     = scrRegion.extent.x;
      region.extent.y     = scrRegion.extent.y;

      // invert the old diver bitmap
      GfxCopyRegion(gbls->winDivers, GfxGetDrawWindow(),
                       &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxMask);
      gbls->diverOnScreen    = FALSE;
    }

    // 
    // draw diver at the new position
    // 

    index = prefs->game.oktopus.diverPosition;

    // what is the rectangle we need to copy?
    GameGetSpritePosition(spriteDiver, index, 
                          &scrRegion.topLeft.x, &scrRegion.topLeft.y);
    scrRegion.extent.x  = 26;
    scrRegion.extent.y  = 26;
    region.topLeft.x    = index * scrRegion.extent.x; 
    region.topLeft.y    = 0;
    region.extent.x     = scrRegion.extent.x;
    region.extent.y     = scrRegion.extent.y;

    // save this location, record diver is onscreen
    gbls->diverOnScreen    = TRUE;
    gbls->diverOldPosition = index;

    // draw the diver bitmap!
    GfxCopyRegion(gbls->winDivers, GfxGetDrawWindow(),
                  &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);

    // dont draw until we need to
    gbls->diverChanged = FALSE;
  }
}

/**
 * Terminate the game.
 */
void   
GameTerminate()
{
  // clean up windows/memory
  pfree(gbls->winDigits);
  pfree(gbls->winLives);
  pfree(gbls->winDivers);
  pfree(gbls->winDiverDeaths);
  pfree(gbls->winTentacles);
  pfree(gbls);
}

/**
 * Get the position of a particular sprite on the screen.
 *
 * @param spriteType the type of sprite.
 * @param index the index required in the sprite position list.
 * @param x the x co-ordinate of the position
 * @param y the y co-ordinate of the position
 */
static void
GameGetSpritePosition(BYTE  spriteType, 
                      BYTE  index, 
                      SHORT *x, 
                      SHORT *y)
{
  switch (spriteType) 
  {
    case spriteDigit: 
         {
           *x = 120 + (index * 9);
           *y = 38;
         }
         break;

    case spriteLife: 
         {
           *x = 36;
           *y = 41;
         }
         break;

    case spriteDiver: 
         {
           SHORT positions[][2] = {
                                   {  15,  41 },
                                   {  13,  69 },
                                   {  19,  96 },
                                   {  43, 114 },
                                   {  71, 114 },
                                   { 100, 117 },
                                   { 128, 112 },
                                   { 100, 117 },
                                   { 100, 117 }
                                  };

           *x = positions[index][0];
           *y = positions[index][1];
         }
         break;

    case spriteDiverDeath: 
         {
           SHORT positions[][2] = {
                                   {  77,  86 },
                                   {  77,  86 }
                                  };

           *x = positions[index][0];
           *y = positions[index][1];
         }
         break;

    case spriteTentacle: 
         {
           SHORT positions[][2] = {
                                   {  51,  76 }, // tentacle one
                                   {  40,  77 },
                                   {  31,  73 },
                                   {   0,   0 },
                                   {   0,   0 },
                                   {  51,  76 }, // tentacle two
                                   {  44,  82 },
                                   {  40,  88 },
                                   {  36,  97 },
                                   {   0,   0 },
                                   {  68,  85 }, // tentacle three
                                   {  66,  91 },
                                   {  65,  97 },
                                   {  64, 105 },
                                   {  60, 112 },
                                   {  87,  94 }, // tentacle four
                                   {  88, 101 },
                                   {  89, 106 },
                                   {  89, 114 },
                                   {   0,   0 },
                                   { 116, 102 }, // tentacle five
                                   { 120, 110 },
                                   { 123, 115 },
                                   {   0,   0 },
                                   {   0,   0 },
                                   {  72,  98 }  // death hold
                                  };

           *x = positions[index][0];
           *y = positions[index][1];
         }
         break;

    default:
         break;
  }
}

/**
 * Adjust the level (reset positions)
 *
 * @param prefs the global preference data.
 */
static void 
GameAdjustLevel(PreferencesType *prefs)
{
  // reset the "gold" count
  prefs->game.oktopus.goldInBag         = 0;

  // player should go back to the "starting" position
  prefs->game.oktopus.diverCount        = 0;
  prefs->game.oktopus.diverPosition     = 0;
  prefs->game.oktopus.diverNewPosition  = prefs->game.oktopus.diverPosition;
  gbls->diverChanged                    = TRUE;

  // player is not dead
  gbls->playerDied                      = FALSE;
}

/**
 * Increment the players score. 
 *
 * @param prefs the global preference data.
 */
static void 
GameIncrementScore(PreferencesType *prefs)
{
  SHORT      i, index;
  GfxRegion  region     = { {   0,   0 }, {   0,   0 } };
  GfxRegion  scrRegion  = { {   0,   0 }, {   0,   0 } };

  // adjust accordingly
  prefs->game.gameScore += prefs->game.oktopus.bonusScoring ? 2 : 1;

  // redraw score bitmap
  {
    SHORT base;
 
    base = 1000;  // max score (4 digits)
    for (i=0; i<4; i++) {

      index = (prefs->game.gameScore / base) % 10;

      // what is the rectangle we need to copy?
      GameGetSpritePosition(spriteDigit, i, 
                            &scrRegion.topLeft.x, &scrRegion.topLeft.y);
      scrRegion.extent.x  = 7;
      scrRegion.extent.y  = 12;
      region.topLeft.x    = index * scrRegion.extent.x;
      region.topLeft.y    = 0;
      region.extent.x     = scrRegion.extent.x;
      region.extent.y     = scrRegion.extent.y;

      // draw the digit!
      GfxCopyRegion(gbls->winDigits, GfxGetDrawWindow(),
                    &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxPaint);
      base /= 10;
    }
  }

  // play the sound
  SndPlaySndEffect(SNDRES5_BEEP);
  TmrWaitTime(125);

  // is it time for a bonus?
  if (
      (prefs->game.gameScore >= 300) &&
      (prefs->game.oktopus.bonusAvailable)
     ) {

    // little fan-fare :)) - the "veewoo" sound was the best i could find :((
    SndPlaySndEffect(SNDRES_VEEWOO);
    TmrWaitTime(500);

    // apply the bonus!
    if (prefs->game.gameLives == 3) 
      prefs->game.oktopus.bonusScoring = TRUE;
    else
      prefs->game.gameLives = 3;

    prefs->game.oktopus.bonusAvailable = FALSE;
  }
}

/**
 * Move the player.
 *
 * @param prefs the global preference data.
 */
static void
GameMovePlayer(PreferencesType *prefs) 
{
  // 
  // collecting bonus points for returning to the boat?
  //

  if (
      (prefs->game.oktopus.diverPosition == 0) &&
      (prefs->game.oktopus.goldInBag     != 0)
     ) {

    GfxRegion  region     = { {   0,   0 }, {   0,   0 } };
    GfxRegion  scrRegion  = { {   0,   0 }, {   0,   0 } };
    SHORT      i;

    if (gbls->diverOnScreen) {

      // what is the rectangle we need to copy?
      GameGetSpritePosition(spriteDiver, 0, 
                            &scrRegion.topLeft.x, &scrRegion.topLeft.y);
      scrRegion.extent.x  = 26;
      scrRegion.extent.y  = 26;
      region.topLeft.x    = 0;
      region.topLeft.y    = 0;
      region.extent.x     = scrRegion.extent.x;
      region.extent.y     = scrRegion.extent.y;

      // invert the old diver bitmap
      GfxCopyRegion(gbls->winDivers, GfxGetDrawWindow(),
                    &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxMask);
    }

    // what is the rectangle we need to copy?
    GameGetSpritePosition(spriteDiver, 0, 
                          &scrRegion.topLeft.x, &scrRegion.topLeft.y);
    scrRegion.extent.x  = 26;
    scrRegion.extent.y  = 26;
    region.topLeft.x    = 0;
    region.topLeft.y    = 2 * scrRegion.extent.y;
    region.extent.x     = scrRegion.extent.x;
    region.extent.y     = scrRegion.extent.y;

    // draw the "collecting" diver bitmap
    GfxCopyRegion(gbls->winDivers, GfxGetDrawWindow(),
                  &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);

    // this is worth 3 points
    for (i=0; i<3; i++) {
      GameIncrementScore(prefs);
    }

    // erase the "collecting" diver bitmap
    GfxCopyRegion(gbls->winDivers, GfxGetDrawWindow(),
                  &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxMask);

    if (gbls->diverOnScreen) {

      // what is the rectangle we need to copy?
      GameGetSpritePosition(spriteDiver, 0, 
                            &scrRegion.topLeft.x, &scrRegion.topLeft.y);
      scrRegion.extent.x  = 26;
      scrRegion.extent.y  = 26;
      region.topLeft.x    = 0;
      region.topLeft.y    = 0;
      region.extent.x     = scrRegion.extent.x;
      region.extent.y     = scrRegion.extent.y;

      // draw the old diver bitmap
      GfxCopyRegion(gbls->winDivers, GfxGetDrawWindow(),
                    &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);
    }

    // reset elementary information
    prefs->game.oktopus.goldInBag  = 0;
    prefs->game.oktopus.diverCount = 0;
  }

  // 
  // do we "boot" the diver out from the boat?
  //

  if (
      (prefs->game.oktopus.diverPosition == 0) &&
      ((prefs->game.oktopus.diverCount / GAME_FPS) == 5)  // 5 seconds
     ) {
    prefs->game.oktopus.diverNewPosition = 1;
  }

  //
  // where does diver want to go today?
  //

  // current position differs from new position?
  if (prefs->game.oktopus.diverPosition != 
      prefs->game.oktopus.diverNewPosition) {

    // need to move left
    if (prefs->game.oktopus.diverPosition > 
        prefs->game.oktopus.diverNewPosition) {

      gbls->moveNext = moveLeft;
    }

    // need to move right
    else
    if (prefs->game.oktopus.diverPosition < 
        prefs->game.oktopus.diverNewPosition) {

      gbls->moveNext = moveRight;
    }
  }

  // lets make sure they are allowed to do the move
  if (
      (gbls->moveDelayCount == 0) || 
      (gbls->moveLast != gbls->moveNext) 
     ) {
    gbls->moveDelayCount = 
     ((gbls->gameType == GAME_A) ? 4 : 3);
  }
  else {
    gbls->moveDelayCount--;
    gbls->moveNext = moveNone;
  }

  // update counter
  prefs->game.oktopus.diverCount++;

  // which direction do they wish to move?
  switch (gbls->moveNext)
  {
    case moveLeft:
         {
           // lets make sure it is valid
           if (
               (prefs->game.oktopus.diverPosition != 1) ||
               (prefs->game.oktopus.goldInBag      > 0)
              ) {
             prefs->game.oktopus.diverPosition--;
             gbls->diverChanged = TRUE;
           }
         }
         break;

    case moveRight:
         {
           // collecting "gold"?
           if (prefs->game.oktopus.diverPosition == 5) {

             // max 5 gold pieces in the bag at once
             if (prefs->game.oktopus.goldInBag < 5) {

               GfxRegion  region     = { {   0,   0 }, {   0,   0 } };
               GfxRegion  scrRegion  = { {   0,   0 }, {   0,   0 } };
               SHORT      i, index;

               // erase the "old" diver
               if (gbls->diverOnScreen) {

                 index = gbls->diverOldPosition;

                 // what is the rectangle we need to copy?
                 GameGetSpritePosition(spriteDiver, index, 
                                       &scrRegion.topLeft.x, &scrRegion.topLeft.y);
                 scrRegion.extent.x  = 26;
                 scrRegion.extent.y  = 26;
                 region.topLeft.x    = index * scrRegion.extent.x; 
                 region.topLeft.y    = 0;
                 region.extent.x     = scrRegion.extent.x;
                 region.extent.y     = scrRegion.extent.y;

                 // invert the old diver bitmap
                 GfxCopyRegion(gbls->winDivers, GfxGetDrawWindow(),
                               &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxMask);
               }
  
               for (i=0; i<2; i++) {

                 // what is the rectangle we need to copy?
                 GameGetSpritePosition(spriteDiver, 7+i, 
                                       &scrRegion.topLeft.x, &scrRegion.topLeft.y);
                 scrRegion.extent.x  = 26;
                 scrRegion.extent.y  = 26;
                 region.topLeft.x    = i * scrRegion.extent.x; 
                 region.topLeft.y    = 1 * scrRegion.extent.y;
                 region.extent.x     = scrRegion.extent.x;
                 region.extent.y     = scrRegion.extent.y;
   
                 // draw the diver bitmap
                 GfxCopyRegion(gbls->winDivers, GfxGetDrawWindow(),
                               &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);

                 // wait one frame
                 TmrWaitTime(1000 / GAME_FPS);
  
                 // invert the diver bitmap
                 GfxCopyRegion(gbls->winDivers, GfxGetDrawWindow(),
                               &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxInvert);
               }

               // ok! one more gold coin collected :)
               GameIncrementScore(prefs);
               prefs->game.oktopus.goldInBag++;  // on more gold :))

               // draw the "old" diver
               if (gbls->diverOnScreen) {

                 index = gbls->diverOldPosition;

                 // what is the rectangle we need to copy?
                 GameGetSpritePosition(spriteDiver, index, 
                                       &scrRegion.topLeft.x, &scrRegion.topLeft.y);
                 scrRegion.extent.x  = 26;
                 scrRegion.extent.y  = 26;
                 region.topLeft.x    = index * scrRegion.extent.x; 
                 region.topLeft.y    = 0;
                 region.extent.x     = scrRegion.extent.x;
                 region.extent.y     = scrRegion.extent.y;

                 // draw the old diver bitmap
                 GfxCopyRegion(gbls->winDivers, GfxGetDrawWindow(),
                               &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);
               }
             }

             // stay put :)
             prefs->game.oktopus.diverNewPosition = 
               prefs->game.oktopus.diverPosition;
           }

           else {
             prefs->game.oktopus.diverPosition++;
             gbls->diverChanged = TRUE;
           }
         }
         break;

    default:
         break;
  }

  gbls->moveLast = gbls->moveNext;
  gbls->moveNext = moveNone;

  // do we need to play a movement sound? 
  if (gbls->diverChanged)  
    SndPlaySndEffect(SNDRES0_BEEP);
}

/**
 * Move the tentacles.
 *
 * @param prefs the global preference data.
 */
static void
GameMoveTentacles(PreferencesType *prefs) 
{
  // only do this if the player is still alive
  if (!gbls->playerDied) {

    BYTE  tentacleLength[] = { 3, 4, 5, 4, 3 };
    SHORT i;

    // process each "tentacle"
    for (i=0; i<5; i++) {

      // is it time to process it?
      if (prefs->game.oktopus.tentacleWait[i] == 0) {

        // start moving the tentacle?
        if (prefs->game.oktopus.tentaclePosition[i] == 0) {

          BOOLEAN ok;
          BYTE    birthFactor = (gbls->gameType == GAME_A) ? 4 : 2;

          ok = !(
                 (
                  (i == 0) && 
                  ((prefs->game.oktopus.tentaclePosition[1] != 0)  ||
                   (prefs->game.oktopus.diverPosition        > 2))
                 ) ||
                 (
                  (i == 1) && 
                  (prefs->game.oktopus.tentaclePosition[0] != 0)
                 )
                );
          ok &= ((DeviceRandom(0) % birthFactor) == 0);

          // are we going to add?
          if (ok) {

            // its moving :)
            prefs->game.oktopus.tentaclePosition[i]  = 1;
            prefs->game.oktopus.tentacleDirection[i] = 1;
            prefs->game.oktopus.tentacleWait[i]      =
              (gbls->gameType == GAME_A) 
                ? MAX(4, 6 - (prefs->game.oktopus.gameLevel >> 1))
                : MAX(3, 5 - (prefs->game.oktopus.gameLevel >> 2));

            gbls->tentacleChanged[i] = TRUE;
          }
        }

        // its on it's way.. cycle through
        else {

          prefs->game.oktopus.tentaclePosition[i] += 
            prefs->game.oktopus.tentacleDirection[i];
          prefs->game.oktopus.tentacleWait[i]      =
            (gbls->gameType == GAME_A) 
              ? MAX(4, 6 - (prefs->game.oktopus.gameLevel >> 1))
              : MAX(3, 5 - (prefs->game.oktopus.gameLevel >> 2));
          gbls->tentacleChanged[i] = TRUE;

          // has the tentacle reached the end of its path?
          if (prefs->game.oktopus.tentaclePosition[i] == tentacleLength[i])
            prefs->game.oktopus.tentacleDirection[i] = -1;

          // play a movement sound
          SndPlaySndEffect(SNDRES1_BEEP);
        }
      }

      // continue the animations
      else { 

        prefs->game.oktopus.tentacleWait[i]--;

        // has the player been caught?
        gbls->playerDied |=
          (
           ((i+1) == prefs->game.oktopus.diverPosition) &&
           (prefs->game.oktopus.tentaclePosition[i] == tentacleLength[i])
          );
      }
    }
  }
}
