/*
 * @(#)device.c
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
  BYTE colorMode;
  WORD randomSeed;
} DeviceGlobals;

// globals reference
static DeviceGlobals *globals;

// interface
static void DeviceGetEventInt();

/**
 * Initialize the device.
 */
void 
DeviceInitialize()
{
  // create a globals object
  globals = (DeviceGlobals *)pmalloc(sizeof(DeviceGlobals));

  // ok, if running 1.1.08 or better.. go to grayscale mode
  {
    BYTE osVersion[16];

    DeviceGetOSVersionNumber(osVersion);
    if (strncmp(osVersion, "1.1.08", 6) >= 0) {
      LcdGetColorMode(globals->colorMode);
      LcdSetColorMode(GREYSCALE_MODE);
    }
  }

  // set the random seed (based on the current time)
  {
    RTM timer;
    RtcGetTime(&timer);
    globals->randomSeed = ((timer.sec * 1000) + timer.msec) + 1;
  }
}

/**
 * Get an event from the event queue.
 * 
 * @param event the event generated.
 * @param timeout the timeout period (milliseconds), timeout = NULL_EVENT.
 */
void
DeviceGetEvent(EvtType *event, SHORT timeout)
{
  // normal event handling (for helio - wait forever, no timeout)
  if (timeout == evtWaitForever)  
    EvtGetEvent(event);

  // ok, timeout time :)) lets work baby
  else {

    UWORD tmrTimeout = NULL;
    
    // trigger a timer interrupt after "timeout" millisecs
    tmrTimeout = TmrIntEnable(timeout, DeviceGetEventInt);

    // get the event
    EvtGetEvent(event);

    // clean up timer code
    if (tmrTimeout) {
      TmrIntDisable(tmrTimeout);
      tmrTimeout = NULL;
    }
  }
}

/**
 * Get the OS version number for the device.
 *
 * @param osVersion a string to store the version string into.
 */
void
DeviceGetOSVersionNumber(BYTE *osVersion)
{
  BYTE *tmpString;

  // allocate memory for the version
  tmpString = (BYTE *)pmalloc(16 * sizeof(BYTE));

  // 1.1.08 and greater call
  SysGetOSVersionNo(tmpString);

  // if we dont get a nice response, lets try the 1.1.03 way :)
  if ((tmpString[0] < '0') || (tmpString[0] > '9') || (tmpString[1] != '.'))
    SysGetOSVersionNo(&tmpString);

  // transfer the "version" string
  strcpy(osVersion, tmpString);

  // clean up
  pfree(tmpString);
}

/**
 * Get the application preferences.
 *
 * @param prefs a pointer to the preference structure, NULL to get the size.
 * @param prefSize a pointer to a variable containing / to put the size.
 * @param saved are they saved preferences?
 * @return noPreferenceFound or size of preferences.
 */
SHORT
DeviceGetAppPreferences(void *prefs, SHORT *prefSize, BOOLEAN saved)
{
  SHORT      result;
  DatabaseID dbID;

  // do we have a preferences database?
  if (DataFindDB((BYTE*)("OPUS_preferences"),&dbID)) {

    UWORD    numRec, fldSize;
    USHORT   fldNumber;
    RecordID recID;

    // open the database
    DataOpenDB(dbID,0,OPEN_RW);

    // find the record
    DataTotalRecord(dbID, &numRec);
    if (numRec != 0) {

      DataOpenRecord(dbID,0,&recID,NULL);
      fldNumber = (saved) ? 0 : 1;

      // are we supposed to find out the size of the existing preference?
      if (prefs == NULL) {
        DataFieldSize(dbID,recID,fldNumber,&fldSize);
        *prefSize = fldSize;
        result    = *prefSize;
      }

      // we need to read it in!
      else {

        BYTE*  buffer;
        UWORD  bufSize;

        DataGetField(dbID,recID,fldNumber,&buffer,&bufSize);
        if (bufSize <= *prefSize) {
          *prefSize = bufSize;
          memcpy(prefs,buffer,bufSize);
        }
        qfree(buffer);

        result = bufSize;
      }

      // close the record
      DataCloseRecord(dbID,recID);
    }
    else 
      result = noPreferenceFound;

    // close the database
    DataCloseDB(dbID);
  }

  // unable to find the preferences database
  else 
    result = noPreferenceFound;

  return result;
}

/**
 * Set the application preferences.
 *
 * @param prefs a pointer to the preference structure.
 * @param prefSize the size of the preference structure.
 * @param saved are they saved preferences?
 */
void
DeviceSetAppPreferences(void *prefs, SHORT prefSize, BOOLEAN saved)
{
  DatabaseID dbID;
  USHORT     fldNumber;
  UWORD      numRec;
  RecordID   recID;

  //
  // open the database
  //

  // database exists? create it if not :))
  if (!DataFindDB((BYTE*)("OPUS_preferences"),&dbID)) {
    DataNewDB((BYTE*)("OPUS_preferences"),1,(BYTE*)("VTech Information"),&dbID);
  }
  DataOpenDB(dbID,0,OPEN_RW);

  // open the record
  DataTotalRecord(dbID, &numRec);
  if (numRec == 0) {
    DataNewRecord(dbID,0,2,&recID);
  }
  DataOpenRecord(dbID,0,&recID,NULL);
  
  // write the record chunk
  fldNumber = (saved) ? 0 : 1;
  DataWriteField(dbID,recID,fldNumber,prefSize,(BYTE*)prefs);

  // close the record
  DataCloseRecord(dbID,recID);

  // close the database
  DataCloseDB(dbID);
}

/**
 * Generate a random number.
 *
 * @param seed value, 0 to use existing seed.
 * @return random number.
 */
SHORT
DeviceRandom(WORD seed)
{
  if (seed != 0) globals->randomSeed = seed;
  globals->randomSeed = (0x015A4E35L * globals->randomSeed) + 1;
  return (globals->randomSeed >> 16) & 0x7FFF;
}

/**
 * Determine the key press state of the device.
 *
 * @return the key press state of the device.
 */
UWORD
DeviceKeyCurrentState()
{
  UWORD result = 0;

  {
    UWORD pinPower, pinKeys;

    // get the key status (from CPU)
    pinPower = CPU->REG_POWER_CTRL;
    pinKeys  = (CPU->REG_IO_CTRL & READ_IO_KEY1TO7);

    // map the results as appropriate :)
    if (pinKeys & 0x01) result       |= keyBitHard1;
    if (pinKeys & 0x02) result       |= keyBitHard2;
    if (pinKeys & 0x04) result       |= keyBitHard3;
    if (pinKeys & 0x08) result       |= keyBitPageDown;
    if (pinKeys & 0x10) result       |= keyBitPageUp;
    if (pinPower & POWER_REG) result |= keyBitPower;
  }

  return result;
}

/**
 * Reset the device to its original state.
 */
void 
DeviceTerminate()
{
  // ok, if running 1.1.08 or better.. restore the gfx mode
  {
    BYTE osVersion[16];

    DeviceGetOSVersionNumber(osVersion);
    if (strncmp(osVersion, "1.1.08", 6) >= 0) {
      LcdSetColorMode(globals->colorMode);
    }
  }

  // clean up memory
  pfree(globals);
}

/**
 * Callback Interrupt for DeviceGetEvent() - to add NULL_EVENT to queue.
 */
static void DeviceGetEventInt()
{
  EvtAppendEvt(NULL_EVENT,0,0,0,NULL);
}
