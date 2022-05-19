/*
 * @(#)syscall.s
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

#include <pr3910.h>

/**
 ** vt-os 1.1
 **
 ** -- Aaron Ardiri, 2000
 **/

#define SYSCALL(name,number)                       \
        .globl   name;                             \
        .ent     name;                             \
name:;                                             \
        .set     noat;                             \
        li       v0, number;                       \
        syscall;                                   \
        j        ra;                               \
        .set     at;                               \
        .end     name;                             \

        .text
SYSCALL(EvtGetEvent                                ,9)
SYSCALL(EvtAppendEvt                               ,14)
SYSCALL(TmrIntEnable                               ,16)
SYSCALL(TmrIntDisable                              ,17)
SYSCALL(TmrWaitTime                                ,18)
SYSCALL(RtcGetTime                                 ,19)
SYSCALL(RtcDiffTime                                ,21)
SYSCALL(pmalloc                                    ,48)
SYSCALL(pfree                                      ,50)
SYSCALL(qfree                                      ,53)
SYSCALL(DataNewRecord                              ,54)
SYSCALL(DataOpenRecord                             ,58)
SYSCALL(DataCloseDB                                ,59)
SYSCALL(DataCloseRecord                            ,62)
SYSCALL(DataWriteField                             ,70)
SYSCALL(DataGetField                               ,72)
SYSCALL(DataNewDB                                  ,78)
SYSCALL(DataFindDB                                 ,79)
SYSCALL(DataOpenDB                                 ,80)
SYSCALL(DataFieldSize                              ,89)
SYSCALL(DataTotalRecord                            ,103)
SYSCALL(strlen                                     ,119)
SYSCALL(strcpy                                     ,121)
SYSCALL(_FormDispatchEvent                         ,224)
SYSCALL(FormDrawForm                               ,226)
SYSCALL(FormSetEventHandler                        ,227)
SYSCALL(FormGetActiveFormID                        ,228)
SYSCALL(FormSetActiveForm                          ,229)
SYSCALL(FormGetObjectPointer                       ,233)
SYSCALL(FormGotoForm                               ,234)
SYSCALL(FormSaveBehindBits                         ,252)
SYSCALL(FormRestoreBitBehind                       ,253)
SYSCALL(FormInitForm                               ,255)
SYSCALL(MenuHandleEvent                            ,262)
SYSCALL(ltoa                                       ,321)
SYSCALL(memcpy                                     ,322)
SYSCALL(ScrollbarEraseScrollbar                    ,353)
SYSCALL(ScrollbarGetScrollbar                      ,355)
SYSCALL(ScrollbarSetScrollbar                      ,356)
SYSCALL(ScrollbarDrawScrollbar                     ,363)
SYSCALL(SystemHandleEvent                          ,375)
SYSCALL(UISearchForAddress                         ,459)
SYSCALL(UIDeleteAllAppObjects                      ,465)
SYSCALL(UIApplicationInit                          ,542)
SYSCALL(memset                                     ,573)
SYSCALL(strncmp                                    ,583)
SYSCALL(SysGetAppID                                ,672) 
SYSCALL(SndPlaySndEffect                           ,734)
SYSCALL(SysGetOSVersionNo                          ,771)

// vt-os 1.1.08

SYSCALL(LcdSetColorMode                            ,856)
SYSCALL(LcdGetColorMode                            ,857)
