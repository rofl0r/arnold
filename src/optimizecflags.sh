#!/bin/sh
#export CFLAGS="-O3 -m486 -Wall -Wno-unused -funroll-loops -fstrength-reduce -fomit-frame-pointer -ffast-math -malign-functions=2 -malign-jumps=2 -malign-loops=2"
export CFLAGS="-O3 -mcpu=i686 -march=i686 -fforce-addr -fomit-frame-pointer -funroll-loops -frerun-cse-after-loop -frerun-loop-opt -malign-functions=4"
