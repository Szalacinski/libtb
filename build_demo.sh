#!/bin/sh
gcc -I. simple_demo.c libtb/libtb.c libtb/internal.c libtb/protocols/serial.c libtb/vendors/tandberg.c -o simple_demo -lserialport -Wall
