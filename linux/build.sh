#!/bin/bash

set -e

BASE_DIR=$(dirname "$0")

rm -rf $BASE_DIR/dist
mkdir $BASE_DIR/dist
cp -R $BASE_DIR/../common/assets $BASE_DIR/dist/assets

gcc \
  -I$BASE_DIR/../../SDL/include \
  -I$BASE_DIR/../../SDL_image \
  -lcurl -lSDL2 -lSDL2_image \
  $BASE_DIR/../common/src/main.c \
  -o $BASE_DIR/dist/boxed
