# Boxed

Proof of concept using SDL2 project with shared code that works on Linux, Android and iOS.

This repository was created for personal study purposes, so you won't find proper replication guidelines. Feel free to ask any questions if you want to know more about the process.

Also keep in mind that `common/src/main.c` contains references to all three environments via C macros. In a maintainable code base this should be abstracted away to their respective platform folders for readability reasons. The intent of the proof of concept was to show that a shared C code base was possible, so designs decisions like this were permitted as long as it made the writing of the code easier and faster.

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

## Build SDL2 and SDL2_image Android Prefabs

(check your Android Home and NDK Home dirs)

```
cd ../SDL
ANDROID_HOME=~/Android/Sdk ANDROID_NDK_HOME=~/Android/Sdk/ndk/21.4.7075529 ./build-scripts/android-prefab.sh
cd ../SDL_image
sdl_build_root=../SDL/build-android-prefab ANDROID_HOME=~/Android/Sdk ANDROID_NDK_HOME=~/Android/Sdk/ndk/21.4.7075529 ./build-scripts/android-prefab.sh
```

## Adapt Android Template

- `gradle.properties`: Use prefab version 2

```
android.prefabVersion=2.0.0
```

- Delete Java Classes (they will be available inside the prefab jars)

```
rm -rf android/app/src/main/java/org/
```

- `app/build.gradle`

  * Clean referentes to Cmake build
  * Clean buildAsLibrary variant
  * Change applicationId
  * Change minSdkVersion to 24
  * Add prefab dependencies (check versions looking for newer releases):

    ```
    dependencies {
        implementation 'com.android.ndk.thirdparty:libpng:1.6.37-alpha-1'
        implementation files('../../../SDL/build-android-prefab/prefab-2.27.0/SDL2-2.27.0.aar')
        implementation files('../../../SDL_image/build-android-prefab/prefab-2.7.0/SDL2_image-2.7.0.aar')
        implementation fileTree(include: ['*.jar'], dir: 'libs')
    }
    ```

  * Enable prefab (right below `lintOptions`)

    ```
    buildFeatures {
        prefab true
    }
    ```

- `AndroidManifest.xml`

  * Change package
  * Change activity android:name attribute
  * Add permission for INTERNET access:

    ```
    <uses-permission android:name="android.permission.INTERNET" />
    ```

- Symlink assets:

```
mkdir android/app/src/main/assets
ln -s $PWD/common/assets $PWD/android/app/src/main/assets/assets
```

(the resulting folder will be android/app/src/main/assets/assets)

- Change app name inside `android/app/src/main/res/values/strings.xml`

- Create Java classes `Boxed` and `HTTPRequests` in package `android/app/src/main/java/dev/samuel/boxed`

- Delete all `CmakeLists.txt` files

- Enable `c++-shared` STL inside `Application.mk`

```
APP_STL := c++_shared
```

- Adapt `app/jni/src/Android.mk`

  * Remove `SDL_PATH` and `LOCAL_C_INCLUDES`
  * Adapt `LOCAL_SRC_FILES` (`common/main.c`)
  * Add `SDL2_image png16` to `LOCAL_SHARED_LIBRARIES`
  * Remove everything but `-llog` from `LOCAL_LDLIBS`
  * Add prefab calls at the end of the file:

    ```
    $(call import-module,prefab/SDL2)
    $(call import-module,prefab/SDL2_image)
    $(call import-module,prefab/png16)
    ```

- Symlink C code:

```
ln -s $PWD/common/src $PWD/android/app/jni/src/common
```

## Running Android App

- Check if you have a simulator or device running (how to run one of them is outside the scope of this README)

```
adb devices
```

- Run it

```
cd android/
./gradlew installDebug
```

- Checking logs

```
adb logcat | grep Boxed
```

## Creating the Xcode Project

- Select the `iOS` tab.
- Select `Game`.
- Fill in Product Name.
- Select the Team.
- Keep language as Objective-C.
- Keep Game Technology as Metal.
- Untick Tests.
- Untick "Create Git repository on my Mac".
- Delete everything except Assets and LaunchScreen.
- Change deployment target to a reasonable value (e.g. 12).
- Clone both SDL and SDL_image as siblings to this repository folder.
- Checkout the `SDL2` branch on both repositories.
- Add both `SDL/Xcode/SDL/SDL.xcodeproj` and `SDL_image/Xcode/SDL_image.xcodeproj` to the project.
- Clear the "Main storyboard file base name" property (Main Target > Info Tab > Custom iOS Target Properties).
- Add both include folders (SDL > Public Headers and SDL_image root folder) to the target's Build Settings > Header Search Path.
- Do the same for the SDL folder on the SDL_image Framework target header search path.
- Add both libraries (SDL > SDL2.framework from Framework-IOS and SDL_image > SDL2_image.framework) to the Build Phases > Link Binary With Libraries section.
- Mark both frameworks to be embedded and signed on General > Frameworks, Libraries and Embedded Content.
- Expand SDL > Library Source > main > uikit and drag SDL_uikit_main.c to the main game group.
- Add a reference to `main.c` from the `common/src` folder.
- Add a reference to `common/assets` as well inside the main project group.

## Useful Commands to Remember

- Related to loading shared libraries on Linux:

```
readelf -d linux/dist/boxed
sudo lsof | grep boxed | grep SDL
objdump -p linux/dist/boxed
LD_LIBRARY_PATH=. ./boxed
```

