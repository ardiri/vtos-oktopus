/*
 * @(#)gccfix.c
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

/**
 * The Helio Computing Platform entry routine (mainline).
 *
 * @param cmd    a word value specifying the launch code.
 * @param cmdPBP pointer to a structure associated with the launch code.
 * @return zero if launch successful, non zero otherwise.
 */
BOOLEAN
__main(WORD cmd, void *cmd_ptr)
{
  BOOLEAN result = FALSE;

  // what type of launch was this?
  switch (cmd) 
  {
    case LAUNCH_CMD_NORMAL_LAUNCH:
         {
           UIApplicationInit();
	   HelioMain(cmd, cmd_ptr);
           UIDeleteAllAppObjects();
         }
         result = TRUE;
         break;

    case LAUNCH_CMD_GOTO_REC:
         if (((GotoRec*)cmd_ptr)->find_string != NULL)
           pfree(((GotoRec*)cmd_ptr)->find_string);

    case LAUNCH_CMD_FIND:
    case LAUNCH_CMD_ALARM_HIT:
         pfree(cmd_ptr);
         result = TRUE;
         break;

    default:
         break;
  }

  return result;
}
