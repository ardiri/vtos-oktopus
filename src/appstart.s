/*
 * @(#)appstart.s
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
#include <pr31700s.h>

#define USER_MEM_BASE        0x10000000

        .data
        .globl header
header:
.word   _ftext   - USER_MEM_BASE
.word   etext    - USER_MEM_BASE
.word   _fdata   - USER_MEM_BASE
.word   edata    - USER_MEM_BASE
.word   _fbss    - USER_MEM_BASE
.word   ebss     - USER_MEM_BASE
.word   res_data - USER_MEM_BASE
.word   0

        .text
        .globl appstart
appstart:
        la     t0, __main
        j      t0
