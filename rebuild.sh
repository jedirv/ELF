#!/bin/bash
export CXX=g++
export CC=gcc
if [ "$CXX" = "g++" ]; then export CXX="g++-4.9" CC="gcc-4.9"; fi
(cd rts/game_MC && make gen && make)
cd rts/backend
make clean && make minirts GAME_DIR=../game_MC
