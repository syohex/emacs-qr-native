UNAME_S=$(shell uname -s)
EMACS_ROOT ?= ../..

EMACS ?= emacs
CC      = gcc
LD      = gcc
CFLAGS = -std=gnu99 -ggdb3 -Wall -fPIC -I$(EMACS_ROOT)/src -Iqrcode/libqr

.PHONY : format clean

all: qr-native-core.so

qr-native-core.so: qr-native-core.o qr.o qrcnv.o qrcnv_bmp.o
	$(LD) -shared -o $@ $^

qr.o: qrcode/libqr/qr.c
	$(CC) $(CFLAGS) -c -o $@ $<

qrcnv.o: qrcode/libqr/qrcnv.c
	$(CC) $(CFLAGS) -c -o $@ $<

qrcnv_bmp.o: qrcode/libqr/qrcnv_bmp.c
	$(CC) $(CFLAGS) -c -o $@ $<

qr-native-core.o: qr-native-core.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm -f qr-native-core.so qr-native-core.o

format:
	clang-format -i *.c
