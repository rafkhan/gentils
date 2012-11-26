CC=clang
CFLAGS= -Wall -g
VERSION=1.0
DATE=$(shell date)

CC=gcc
CFLAGS= -Wall\
		-DCOMPILE_TIME="\"$(DATE)\""\
		-DVERSION="\"$(VERSION)\""


all: whoami.c
	$(CC) $(CFLAGS) whoami.c -o ../../bin/whoami
