#
# @(#)Makefile
#
# Copyright 1999-2001, Aaron Ardiri (mailto:aaron@ardiri.com)
# All rights reserved.
#
# This file was  generated as part of the  "oktopus" program developed 
# for the Helio Computing Platform designed by vtech:
#
#   http://www.vtechinfo.com/
#
# The contents of this file is confidential and  proprietary in nature
# ("Confidential Information"). Redistribution or modification without 
# prior consent of the original author is prohibited.
#

# compiler setup
PREFIX    = 
AS        = $(PREFIX)mips-vtos-as
CC        = $(PREFIX)mips-vtos-gcc
LD        = $(PREFIX)mips-vtos-ld

# source / outputs
LD_OBJS   = appstart.o gccfix.o syscall.o resource/gfx.a
OBJS      = helio.o device.o help.o game.o animate.o resource.o
EXEC      = oktopus

# compiler flags 
ASFLAGS   = -O2
CCFLAGS   = -O2
LDFLAGS   = -T resource/link.cmd

# compile requirements
$(EXEC).app: main.obj
	chop main.obj main.out .head .text .data
	acompile /S:37 $(EXEC).app main.out images/mainicon.bmp "Oktopus"
	cp $(EXEC).app ..
	make clean

main.obj: $(OBJS) $(LD_OBJS)
#	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LD_OBJS) # linker dont like :)
	$(LD) $(LDFLAGS) -o $@ *.o resource/gfx.a 

resource.c: resource.rcp
	rcompile resource.rcp > tmp.out
	bin2hex resource.bin resource.c > tmp.out

# compile rules
%.o: %.s
	cp $< tmp.S; $(AS) -c $(ASFLAGS) tmp.S -o $@
%.o: %.c
	$(CC) -c $(CCFLAGS) $<

# clean-up funtions
clean:
	rm -f *.[oO] *.OBJ *.BIN c.txt resource.c *~ main.* tmp.*

distclean:
	rm -f *.[oO] *.OBJ *.BIN c.txt resource.c *~ main.* *.app tmp.*
