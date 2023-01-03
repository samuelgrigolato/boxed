# Boxed

Step by step guide to prepare a new SDL2 project with shared code that works on Linux, Android and iOS.

## Download and Build SDL

- Clone SDL repository as a sibling folder to `boxed`:

```
git clone git@github.com:libsdl-org/SDL.git
```

- Checkout SDL2 branch:

```
git checkout SDL2
```

- Build it:

```
./configure
make -j8
```

## Download and Build SDL_image

- Clone SDL_image repository as a sibling folder to `boxed`:

```
git clone git@github.com:libsdl-org/SDL_image.git
```

- Download external libraries:

```
./external/download.sh
```

- Build it:

```
./configure
make -j8
```

## VS Code Include Path

Add `../SDL/include` and `../SDL_image` to the workspace setting `C_Cpp.default.includePath`.

## The Common Folder

This folder contains the shared assets between all the target platforms.

- `src/main.c` (SDL2 code exercising rendering, image loading, audio playback and a HTTP request)
- `assets/enemy.png` (sprite texture)
- `assets/hit.wav` (sound effect)

## Build and Run on Linux

```
cd linux
./build.sh
cd dist
./boxed
```

## Copy Android Template from SDL Repo

```
mkdir android/
cp -R ../SDL/android-project/* android
```

## Useful Commands to Remember

- Related to loading shared libraries on Linux:

```
readelf -d linux/dist/boxed
sudo lsof | grep boxed | grep SDL
objdump -p linux/dist/boxed
LD_LIBRARY_PATH=. ./boxed
```
