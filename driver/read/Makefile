# Makefile for building embedded application.
# by Brian Fraser

# Edit this file to compile extra C files into their own programs.
TARGET= readfile
SOURCES= readfile.c


PUBDIR = $(HOME)/cmpt433/public/myApps
OUTDIR = $(PUBDIR)
CROSS_TOOL = arm-linux-gnueabihf-
CC_CPP = $(CROSS_TOOL)g++
CC_C = $(CROSS_TOOL)gcc

CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror

all:
	$(CC_C) $(CFLAGS) $(SOURCES) -o $(OUTDIR)/$(TARGET)  $(LFLAGS)


clean:
	rm -f $(OUTDIR)/$(TARGET)