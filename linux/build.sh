#!/bin/bash

set -e

BASE_DIR=$(dirname "$0")

rm -rf $BASE_DIR/dist
mkdir $BASE_DIR/dist
cp -R $BASE_DIR/../common/assets $BASE_DIR/dist/assets

gcc \
  -I$BASE_DIR/../../SDL/include \
  -I$BASE_DIR/../../SDL_image \
  -L$BASE_DIR/../../SDL/build/.libs \
  -L$BASE_DIR/../../SDL_image/.libs \
  -lSDL2 -lSDL2_image \
  -Wl,-rpath,. \
  $BASE_DIR/../common/src/main.c \
  -o $BASE_DIR/dist/boxed

cp $BASE_DIR/../../SDL/build/.libs/libSDL2-2.0.so.0 $BASE_DIR/dist
cp $BASE_DIR/../../SDL_image/.libs/libSDL2_image-2.0.so.0 $BASE_DIR/dist
