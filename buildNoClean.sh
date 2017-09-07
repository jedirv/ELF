#!/bin/bash
export CXX=g++
export CC=gcc
if [ "$CXX" = "g++" ]; then export CXX="g++-4.9" CC="gcc-4.9"; fi
#(cd rts/game_MC && make gen && make -j 2)
(cd rts/game_MC && make gen && make)
cd rts/backend
#make clean && make -j 2 minirts GAME_DIR=../game_MC
make minirts GAME_DIR=../game_MC
