/*
 * @(#)helio.c
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
  PreferencesType *prefs;
} Globals;

// globals reference
static Globals *globals;

// function prototypes
BOOLEAN (*FormDispatchEvent)(EvtType *Event);

// interface
static BOOLEAN mainFormEventHandler(EvtType *);
static BOOLEAN gameFormEventHandler(EvtType *);
static BOOLEAN infoFormEventHandler(EvtType *);
static BOOLEAN helpFormEventHandler(EvtType *);
static BOOLEAN highFormEventHandler(EvtType *);

/**
 * The Form:mainForm event handling routine.
 *
 * @param event the event to process.
 * @return true if the event was handled, false otherwise.
 */
static BOOLEAN 
mainFormEventHandler(EvtType *event)
{
  BOOLEAN processed = FALSE;

  switch (event->eventType)
  {
    case EVT_FORM_OPEN:
         FormDrawForm(mainForm);

         // draw the "seperators"
         GfxDrawLine(4,142,156,142,gfx_dgray);
         GfxDrawLine(4,143,156,143,gfx_dgray);

         // configure
         EvtAppendEvt(EVT_FORM_CONFIGURE,0,0,0,NULL);

         processed = TRUE;
         break;

    case EVT_FORM_CONFIGURE:
         processed = TRUE;
         break;

    case EVT_BITMAP_SELECT:

         switch (event->eventID) 
         {
           case mainFormHelpBitmap:

                // display the info form [modal dialog]
                ApplicationDisplayDialog(helpForm);

                break;

           case mainFormAboutBitmap:

                // display the info form [modal dialog]
                ApplicationDisplayDialog(infoForm);

                processed = TRUE;
                break;

           case mainFormGameABitmap:

                // goto the game!
                GameResetPreferences(globals->prefs, GAME_A);
                FormGotoForm(gameForm);

                processed = TRUE;
                break;

           case mainFormGameBBitmap:

                // goto the game!
                GameResetPreferences(globals->prefs, GAME_B);
                FormGotoForm(gameForm);

                processed = TRUE;
                break;

           case mainFormGameAHighBitmap:

                globals->prefs->game.gameScore          = 
                  globals->prefs->game.highScore[0];
                globals->prefs->game.oktopus.gameType = GAME_A; 

                // show the high score
                ApplicationDisplayDialog(highForm);

                processed = TRUE;
                break;

           case mainFormGameBHighBitmap:

                globals->prefs->game.gameScore          = 
                  globals->prefs->game.highScore[1];
                globals->prefs->game.oktopus.gameType = GAME_B; 

                // show the high score
                ApplicationDisplayDialog(highForm);

                processed = TRUE;
                break;

           default:
                break;
         }
         break;

    case EVT_FORM_CLOSE:
         processed = TRUE;
         break;

    default:
         break;
  }

  return processed;
}

/**
 * The Form:gameForm event handling routine.
 *
 * @param event the event to process.
 * @return true if the event was handled, false otherwise.
 */
static BOOLEAN 
gameFormEventHandler(EvtType *event)
{
  BOOLEAN processed = FALSE;

  switch (event->eventType)
  {
    case EVT_FORM_OPEN:
         FormDrawForm(gameForm);

         // configure
         EvtAppendEvt(EVT_FORM_CONFIGURE,0,0,0,NULL);

         processed = TRUE;
         break;

    case EVT_FORM_CONFIGURE:
         AnimateStart(GAME_FPS);
         processed = TRUE;
         break;

    case EVT_INLAY_SELECT:

         // which button?
         switch (event->para1) 
         {
           case INLAY_EXIT:

                // what game are we playing?
                switch (globals->prefs->game.oktopus.gameType) 
                {
                  case GAME_A:

                       // did we beat the high score?
                       if (globals->prefs->game.gameScore > 
                           globals->prefs->game.highScore[0]) {

                         // set the new high score
                         globals->prefs->game.highScore[0] =
                           globals->prefs->game.gameScore;

                         // display the high score
                         ApplicationDisplayDialog(highForm);
                       }
                       break;

                  case GAME_B:

                       // did we beat the high score?
                       if (globals->prefs->game.gameScore > 
                           globals->prefs->game.highScore[1]) {

                         // set the new high score
                         globals->prefs->game.highScore[1] =
                           globals->prefs->game.gameScore;

                         // display the high score
                         ApplicationDisplayDialog(highForm);
                       }
                       break;

                  default:
                       break;
                }

                // get out! goto the main screen
                globals->prefs->game.gamePlaying = FALSE; 
                FormGotoForm(mainForm);

                // play a little snd
                SndPlaySndEffect(SNDRES5_BEEP);

                processed = TRUE;
                break; 

           default:
                break; 
         }
         break;

    case EVT_BITMAP_SELECT:

         switch (event->eventID) 
         {
           case gameFormHelpBitmap:

                // display the help form [modal dialog]
                ApplicationDisplayDialog(helpForm);

                break;

           case gameFormAboutBitmap:

                // display the info form [modal dialog]
                ApplicationDisplayDialog(infoForm);

                processed = TRUE;
                break;

           case gameFormPauseBitmap:
                globals->prefs->game.gamePaused = 
                  !globals->prefs->game.gamePaused;

                processed = TRUE;
                break;

           case gameFormResetBitmap:
                GameReset(globals->prefs,
                          globals->prefs->game.oktopus.gameType);

                processed = TRUE;
                break;

           default:
                break;
         }
         break;

    case PEN_EVENT:

         // what type?
         switch (event->eventID) 
         {
           case PEN_DOWN:
           case PEN_MOVE:
           case PEN_UP:

                // within the game play area?
                if (
                    (globals->prefs->game.gamePlaying) &&
                    (event->para1 >   0) && (event->para1 < 160) &&
                    (event->para2 >  16) && (event->para1 < 144)
                   ) {

                  // if the player tapped this area, they want to move!
                  GameProcessStylusInput(globals->prefs,
                                         event->para1, event->para2);
                  processed = TRUE;
                }
                break;

           default:
                break;
         }
         break;

    case NULL_EVENT:
         {
           // draw the game!
           GameDraw(globals->prefs);

           // play the game!
           GameProcessKeyInput(globals->prefs, DeviceKeyCurrentState());
           GameMovement(globals->prefs);
         }

         processed = TRUE;
         break;

    case EVT_FORM_CLOSE:
         AnimateStop();
         processed = TRUE;
         break;

    default:
         break;
  }

  return processed;
}

/**
 * The Form:infoForm event handling routine.
 *
 * @param event the event to process.
 * @return true if the event was handled, false otherwise.
 */
static BOOLEAN 
infoFormEventHandler(EvtType *event)
{
  BOOLEAN processed = FALSE;

  switch (event->eventType)
  {
    case EVT_FORM_OPEN:
         FormDrawForm(infoForm);

         // configure
         EvtAppendEvt(EVT_FORM_CONFIGURE,0,0,0,NULL);

         processed = TRUE;
         break;

    case EVT_FORM_CONFIGURE:
         processed = TRUE;
         break;

    case EVT_CONTROL_SELECT:

         switch (event->eventID) 
         {
           case infoFormOkButton:

                // close the current form
                EvtAppendEvt(EVT_FORM_CLOSE,0,0,0,NULL);

                processed = TRUE;
                break;

           default:
                break;
         }
         break;

    case EVT_INLAY_SELECT:

         // which button?
         switch (event->para1)
         {
           case INLAY_OK:
                // relay the event
                EvtAppendEvt(EVT_CONTROL_SELECT,infoFormOkButton,0,0,NULL);

                // play a little snd
                SndPlaySndEffect(SNDRES5_BEEP);

                processed = TRUE;
                break;

           default:
                break;
         }
         break;

    case EVT_FORM_CLOSE:
         processed = TRUE;
         break;

    default:
         break;
  }

  return processed;
}

/**
 * The Form:helpForm event handling routine.
 *
 * @param event the event to process.
 * @return true if the event was handled, false otherwise.
 */
static BOOLEAN 
helpFormEventHandler(EvtType *event)
{
  BOOLEAN processed = FALSE;

  switch (event->eventType)
  {
    case EVT_FORM_OPEN:
         FormDrawForm(helpForm);

         // initialize
         {
           WORD      val,max,min,pge,tot;
           BYTE      objectType;
           Scrollbar *scrollbar;

           // adjust the scrollbar "draw" pagesize
           ScrollbarGetScrollbar(helpFormScrollbar,&val,&max,&min,&pge,&tot);
           FormGetObjectPointer(helpFormScrollbar, 
                                &objectType, (void **)&scrollbar);

           val = HelpInitialize();
           max = (val > (pge+16)) ? (val-(pge+16)) : 0;
           tot = max+pge;
           val = 0;

           // adjust the scroll bar
           ScrollbarSetScrollbar(helpFormScrollbar,
                                 val,max,min,pge,tot);

           // the scroll bar is visible? adjust "scroll car size"
           if (max > 0) {
             scrollbar->scrollbar_attr.scrollbar_visible = TRUE;
             scrollbar->scrollbar_draw_pagesize          = pge;
             ScrollbarDrawScrollbar(helpFormScrollbar);
           }

           // draw the instructions
           HelpDrawInstructions(val);
         }

         // configure
         EvtAppendEvt(EVT_FORM_CONFIGURE,0,0,0,NULL);

         processed = TRUE;
         break;

    case EVT_FORM_CONFIGURE:
         processed = TRUE;
         break;

    case EVT_IO_KEY_CTRL:

         switch (event->eventID) 
         {
           case EVT_IO_KEY_PRESS:
           case EVT_IO_KEY_REPEAT:

                switch (event->para2)
                {
                  case IO_UP_ARROW:
                       {
                         WORD val,max,min,pge,tot;

                         // adjust the scrollbar "draw" pagesize
                         ScrollbarGetScrollbar(helpFormScrollbar,
                                               &val,&max,&min,&pge,&tot);
                         val = (pge > val) ? 0 : (val-pge);
                         ScrollbarSetScrollbar(helpFormScrollbar,
                                               val,max,min,pge,tot);
                         ScrollbarDrawScrollbar(helpFormScrollbar);

                         // draw the instructions
                         HelpDrawInstructions(val);
                       }
                       processed = TRUE;
                       break;

                  case IO_DOWN_ARROW:
                       {
                         WORD val,max,min,pge,tot;

                         // adjust the scrollbar "draw" pagesize
                         ScrollbarGetScrollbar(helpFormScrollbar,
                                               &val,&max,&min,&pge,&tot);
                         val = (max < (val+pge)) ? max : (val+pge);
                         ScrollbarSetScrollbar(helpFormScrollbar,
                                               val,max,min,pge,tot);
                         ScrollbarDrawScrollbar(helpFormScrollbar);

                         // draw the instructions
                         HelpDrawInstructions(val);
                       }
                       processed = TRUE;
                       break;

                  default:
                       break;
                }
                break;

           default:
                break;
         }
         break;

    case EVT_SCROLLBAR_REPEAT:
    case EVT_SCROLLBAR_SELECT:
         {
           WORD val,max,min,pge,tot;

           // draw the scroll bar
           ScrollbarGetScrollbar(helpFormScrollbar,
                                 &val,&max,&min,&pge,&tot);
           ScrollbarDrawScrollbar(helpFormScrollbar);

           // draw the instructions
           HelpDrawInstructions(val);
         }
         break;

    case EVT_CONTROL_SELECT:

         switch (event->eventID) 
         {
           case helpFormOkButton:

                // close the current form
                EvtAppendEvt(EVT_FORM_CLOSE,0,0,0,NULL);

                processed = TRUE;
                break;

           default:
                break;
         }
         break;

    case EVT_INLAY_SELECT:

         // which button?
         switch (event->para1)
         {
           case INLAY_OK:
                // relay the event
                EvtAppendEvt(EVT_CONTROL_SELECT,helpFormOkButton,0,0,NULL);

                // play a little snd
                SndPlaySndEffect(SNDRES5_BEEP);

                processed = TRUE;
                break;

           default:
                break;
         }
         break;

    case EVT_FORM_CLOSE:

         // clean up
         HelpTerminate();

         processed = TRUE;
         break;

    default:
         break;
  }

  return processed;
}

/**
 * The Form:highForm event handling routine.
 *
 * @param event the event to process.
 * @return true if the event was handled, false otherwise.
 */
static BOOLEAN 
highFormEventHandler(EvtType *event)
{
  BOOLEAN processed = FALSE;

  switch (event->eventType)
  {
    case EVT_FORM_OPEN:
         FormDrawForm(highForm);

         // display the high score/code to the user
         {
           GfxFont currFont;
           BYTE    *str;
           SHORT   x, y;

           currFont = GfxGetFont();

           // initialize
           str = (BYTE *)pmalloc(256 * sizeof(BYTE));

           // labels
           x = 40;
           y = 60;
           GfxSetFont(gfx_palmosBoldFont);
           strcpy(str, "Score:");
           GfxDrawString(str, strlen(str), x, y, gfxPaint);
           y += GfxGetFontHeight();
           strcpy(str, "Game:");
           GfxDrawString(str, strlen(str), x, y, gfxPaint);
           y += GfxGetFontHeight();
           strcpy(str, "Code:");
           GfxDrawString(str, strlen(str), x, y, gfxPaint);
           y += GfxGetFontHeight();

           // values
           x = 88;
           y = 60;
           GfxSetFont(gfx_palmosNormalFont);
           ltoa(globals->prefs->game.gameScore, str);
           GfxDrawString(str, strlen(str), x, y, gfxPaint);
           y += GfxGetFontHeight();
           strcpy(str, (globals->prefs->game.oktopus.gameType == GAME_A) 
                       ? "GAME A" : "GAME B");
           GfxDrawString(str, strlen(str), x, y, gfxPaint);
           y += GfxGetFontHeight();
           if (globals->prefs->game.oktopus.gameType == GAME_B) 
             globals->prefs->game.gameScore += 1000;
           {
             BYTE  code[16] = "KEFCXHGBVUIOQALZ";
             SHORT mask[4]  = { 2, 1, 3, 0 };
             SHORT i, val, num;

             val = globals->prefs->game.gameScore;
             for (i=0; i<4; i++) {
               num    = (val & (0x0F << (4 * mask[i])));
               str[i] = code[(num >> (4 * mask[i])) & 0x0F];
             }
             str[4] = '\0';
           }
           GfxDrawString(str, strlen(str), x, y, gfxPaint);

           // clean up
           GfxSetFont(currFont);
           pfree(str);
         }

         // configure
         EvtAppendEvt(EVT_FORM_CONFIGURE,0,0,0,NULL);

         processed = TRUE;
         break;

    case EVT_FORM_CONFIGURE:
         processed = TRUE;
         break;

    case EVT_CONTROL_SELECT:

         switch (event->eventID) 
         {
           case highFormOkButton:

                // close the current form
                EvtAppendEvt(EVT_FORM_CLOSE,0,0,0,NULL);

                processed = TRUE;
                break;

           default:
                break;
         }
         break;

    case EVT_INLAY_SELECT:

         // which button?
         switch (event->para1)
         {
           case INLAY_OK:
                // relay the event
                EvtAppendEvt(EVT_CONTROL_SELECT,highFormOkButton,0,0,NULL);

                // play a little snd
                SndPlaySndEffect(SNDRES5_BEEP);

                processed = TRUE;
                break;

           default:
                break;
         }
         break;

    case EVT_FORM_CLOSE:
         processed = TRUE;
         break;

    default:
         break;
  }

  return processed;
}

/**
 * The Palm Computing Platform initialization routine.
 */
void  
InitApplication()
{
  // initialize the device
  DeviceInitialize();
  AnimateInitialize();
  GfxInitialize();

  // create the globals reference
  globals = (Globals *)pmalloc(sizeof(Globals));

  // load preferences
  {
    SHORT prefSize;
    SHORT flag;

    // allocate memory for preferences
    globals->prefs = (PreferencesType *)pmalloc(sizeof(PreferencesType));

    // lets see how large the preference is (if it is there)
    prefSize = 0;
    flag     = DeviceGetAppPreferences(NULL, &prefSize, TRUE);
    
    // we have some preferences, maybe a match :)
    if ((flag != noPreferenceFound) && (prefSize == sizeof(PreferencesType))) {

      // extract all the bytes
      DeviceGetAppPreferences(globals->prefs,&prefSize,TRUE);
    }

    // we need to reset the preferences..
    else {

      // set default values
      memset(globals->prefs, (UBYTE)0, prefSize);

      globals->prefs->game.gamePlaying  = FALSE;
      globals->prefs->game.highScore[0] = 100;
      globals->prefs->game.highScore[1] = 100; // default high scores
    }
  }

  // initialize the game environment
  GameInitialize();

  // goto the appropriate form
  if (globals->prefs->game.gamePlaying) {
    globals->prefs->game.gamePaused = TRUE;
    FormGotoForm(gameForm);
  }
  else 
    FormGotoForm(mainForm);
}

/**
 * The application event handling routine.
 *
 * @param event the event to process.
 * @return true if the event was handled, false otherwise.
 */
BOOLEAN 
ApplicationHandleEvent(EvtType *event)
{
  BOOLEAN processed = FALSE;

  switch (event->eventType)
  {
    case EVT_FORM_LOAD:
         {
           ObjectID formID, currFormID;
           Form     *frm;
           BYTE     objectType;
           Err      error;

           // initialize the form
           formID = (ObjectID)event->eventID;
           error  = UISearchForAddress(formID, &objectType, (void **)&frm);
           if ((event->para1 == 1) || (error != TRUE)) {
             FormInitForm(formID);
           }

           // load it
           if (UISearchForAddress(formID, &objectType, (void **)&frm)) {

             switch (formID) 
             {
               case mainForm:
                    FormSetEventHandler(mainForm, 
                                        (void **)&FormDispatchEvent, 
                                        (void *)mainFormEventHandler);
                    break;

               case gameForm:
                    FormSetEventHandler(gameForm, 
                                        (void **)&FormDispatchEvent, 
                                        (void *)gameFormEventHandler);
                    break;

               case infoForm:
                    FormSetEventHandler(infoForm, 
                                        (void **)&FormDispatchEvent, 
                                        (void *)infoFormEventHandler);
                    break;

               case helpForm:
                    FormSetEventHandler(helpForm, 
                                        (void **)&FormDispatchEvent, 
                                        (void *)helpFormEventHandler);
                    break;

               case highForm:
                    FormSetEventHandler(highForm, 
                                        (void **)&FormDispatchEvent, 
                                        (void *)highFormEventHandler);
                    break;

               default:
                    break;
             }

             // make it active
             FormSetActiveForm(formID);
             processed = TRUE;
           }
         }
         break;
         
    case EVT_IO_KEY_CTRL:

         switch (event->eventID) 
         {
           case EVT_IO_KEY_PRESS:
           case EVT_IO_KEY_REPEAT:

                // does the form wanna handle it?
                processed = FormDispatchEvent(event);

                // ok, if it is one of the following, we want to ignore it
                switch (event->para2)
                {
                  case IO_PHONE_BOOK:
                  case IO_SCHEDULER:
                  case IO_TO_DO_LIST:
                  case IO_UP_ARROW:
                  case IO_DOWN_ARROW:

                       // we are playing! ignore all these keys :P 
                       processed = TRUE;
                       break;

                  default:
                       break;
                }
                break;

           default:
                break;
         }
         break;

    case EVT_INLAY_SELECT:

         // which button?
         switch (event->para1) 
         {
           case INLAY_OK:

                // does the form wanna handle it?
                processed = FormDispatchEvent(event);

                // no process? relay the event
                if (!processed) {
                  EvtAppendEvt(EVT_INLAY_SELECT,0,INLAY_EXIT,0,NULL);
                  processed = TRUE;
                }
                break; 

           case INLAY_EXIT:

                // does the form wanna handle it?
                processed = FormDispatchEvent(event);

                // no process? relay the event
                if (!processed) {
                  EvtAppendEvt(EVT_INLAY_SELECT,0,INLAY_MAIN_MENU,0,NULL);
                  processed = TRUE;
                }
                break;

           case INLAY_MAIN_MENU:

                // play a little snd
                SndPlaySndEffect(SNDRES5_BEEP);

                // close the current form
                EvtAppendEvt(EVT_FORM_CLOSE,0,0,0,NULL);

                // send an app-stop event
                EvtAppendEvt(EVT_APP_STOP,0,0,0,NULL);

                // lets return to the launcher
                {
                  AppID appID;
                  SysGetAppID((BYTE *)("Mainmenu"), &appID);
                  EvtAppendEvt(EVT_APP_LAUNCH,appID,0,0,NULL);
                }
                processed = TRUE;
                break;

           default:
                break;
         }
         break;

    default:
         break;
  }

  return processed;
}

/**
 * Display a MODAL dialog to the user.
 *
 * @param formID the ID of the form to display.
 */
void
ApplicationDisplayDialog(ObjectID formID)
{
  ObjectID   prevFormID;
  BOOLEAN    appStop, powerDown;
  EvtType    bufEvent;
  GfxWindow *currWindow;

  // save the active form/window
  FormGetActiveFormID(&prevFormID);
  FormSaveBehindBits(prevFormID);
  currWindow = GfxGetDrawWindow();

  GfxSetDrawWindow(GfxGetDisplayWindow());
  appStop   = FALSE;
  powerDown = FALSE;
  {
    BOOLEAN keepFormOpen;
    EvtType event;

    // send load form and open form events
    EvtAppendEvt(EVT_FORM_LOAD, formID, 0, 0, NULL);
    EvtAppendEvt(EVT_FORM_OPEN, formID, 0, 0, NULL);

    // handle all events here (trap them before the OS does) :)
    keepFormOpen = TRUE;
    while (keepFormOpen) {

      EvtGetEvent(&event);

      // this is our exit condition! :)
      keepFormOpen = (event.eventType != EVT_FORM_CLOSE);

      // we have to process the following events in a special way
      if ((event.eventType == EVT_INLAY_SELECT) &&
          (event.para1     == INLAY_MAIN_MENU)) {

        // close the dialog
        EvtAppendEvt(EVT_FORM_CLOSE,0,0,0,NULL);

        // relay the main menu event
        EvtAppendEvt(EVT_INLAY_SELECT,0,INLAY_MAIN_MENU,0,NULL);
      }
      else
      if ((event.eventType == EVT_INLAY_SELECT) &&
          (event.para1     == INLAY_EXIT)) {

        // relay it as an "OK" button press
        EvtAppendEvt(EVT_INLAY_SELECT,0,INLAY_OK,0,NULL);
      }

      // handle the event as required
      else
      if (!ApplicationHandleEvent(&event)) 
        if (!SystemHandleEvent(&event)) 
          if (!MenuHandleEvent(&event)) 
            FormDispatchEvent(&event);

      // stop the app?
      if (event.eventType == EVT_APP_STOP) {

        keepFormOpen = FALSE;
        appStop      = TRUE;

        DeviceGetEvent(&bufEvent, 0);

        // is there a power-down event coming?
        if (bufEvent.eventType == EVT_POWER_DOWN)
          powerDown = TRUE;
      }
    }
  }

  // restore the active form/window
  FormRestoreBitBehind(prevFormID);
  FormSetActiveForm(prevFormID);

  // if not exiting app, return to the previous app
  if (!appStop) {
    EvtAppendEvt(EVT_FORM_LOAD,prevFormID,0,0,NULL);
    EvtAppendEvt(EVT_FORM_CONFIGURE,0,0,0,NULL);
  }
  // need to add the "APP_STOP" event?
  else {
    EvtAppendEvt(EVT_APP_STOP, 0, 0, 0, NULL);

    // need to add the "POWER_DOWN" event?
    if (powerDown) EvtAppendEvt(EVT_POWER_DOWN, 0, 0, 0, NULL);
    else           EvtAppendEvt(bufEvent.eventType,bufEvent.eventID,
                                bufEvent.para1,bufEvent.para2,bufEvent.evtPBP);
  }

  GfxSetDrawWindow(currWindow);
}

/**
 * The Helio Computing Platform event processing loop.
 */
void  
EventLoop()
{
  EvtType event;

  do {

    EvtGetEvent(&event);

    if (!ApplicationHandleEvent(&event)) 
      if (!SystemHandleEvent(&event)) 
        if (!MenuHandleEvent(&event)) 
          FormDispatchEvent(&event);

  } while (event.eventType != EVT_APP_STOP);
}

/**
 * The Helio Computing Platform termination routine.
 */
void  
EndApplication()
{
  // terminate the game environment
  GameTerminate();

  // save preferences
  DeviceSetAppPreferences(globals->prefs,sizeof(PreferencesType),TRUE);

  // clean up
  pfree(globals->prefs);
  pfree(globals);

  // reset the device to its normal state
  GfxTerminate();
  AnimateTerminate();
  DeviceTerminate();
}

/**
 * The Helio Computing Platform entry routine (mainline).
 *
 * @param cmd    a word value specifying the launch code.
 * @param cmdPBP pointer to a structure associated with the launch code.
 * @return zero if launch successful, non zero otherwise.
 */
BOOLEAN
HelioMain(WORD cmd, void *cmd_ptr)
{
  BOOLEAN result = FALSE;

  // what type of launch was this?
  switch (cmd) 
  {
    case LAUNCH_CMD_NORMAL_LAUNCH:
         {
           InitApplication();
           EventLoop();
           EndApplication();
         }
         result = TRUE;
         break;

    default:
         break;
  }

  return result;
}
