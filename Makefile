# VNC Reflector
#
# Copyright (C) 2017 alejandro_liu@hotmail.com.  All rights reserved.
# Copyright (C) 2001-2003 HorizonLive.com, Inc.  All rights reserved.
#
# This software is released under the terms specified in the file LICENSE,
# included.
#
CC = gcc
LD = gcc
PEDANTIC=-Wall -Wextra -Werror -std=gnu99 -pedantic
INCDIRS = -I.

# Use poll(2) syscall in async I/O instead of select(2)
#CONFFLAGS =	-DUSE_POLL

GOPTZ = -Og
OPTIMIZ = -g -D_DEBUG $(GOPTZ) # Debug build
#OPTIMIZ = -O3 # Prod build

# Production
CFLAGS = $(OPTIMIZ) $(PEDANTIC) $(INCDIRS) $(CONFFLAGS)

# Link with zlib and JPEG libraries
LDFLAGS =	-lz -ljpeg

PROG = 	reflector
OBJS = 	main.o logging.o active.o actions.o host_connect.o d3des.o rfblib.o \
	async_io.o host_io.o client_io.o encode.o region.o translate.o \
	control.o encode_tight.o decode_hextile.o decode_tight.o fbs_files.o \
	region_more.o utils.o

#	

default: $(PROG)

$(PROG): vcheck $(OBJS)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJS) $(LDFLAGS)

vcheck:
	: VCHECK
#	if type git ; then \
#	  vname="$$(git describe --dirty=_Exp)" ; \
#	  if [ -n "$$vname" ] ; then \
#	    echo "const char version[] = \"$$vname\";" > version.h.t ;\
#	    if cmp version.h.t version.h ; then \
#	      rm -f version.h.t ; \
#	    else \
#	      mv version.h.t version.h ; \
#	    fi ;\
#	  fi ;\
#	fi

# pull in dependancy...
-include $(OBJS:.o=.d)

%.o: %.c
	# compile and generate dependancy info
	$(CC) -c $(CFLAGS) $*.c -o $*.o
	$(CC) -MM $(CFLAGS) $*.c > $*.d

clean:
	rm -f *.o *.d
	rm -f $(PROG)

