
#ifdef __ANDROID__
/*
  The line below solves the following compilation problem:
    /home/samuel/Android/Sdk/ndk/21.4.7075529/toolchains/llvm/prebuilt/linux-x86_64/lib64/clang/9.0.9/include/mmintrin.h:525:12: error: invalid conversion between vector type '__m64' (vector of 1 'long long' value) and integer type 'int' of different size
      return (__m64)__builtin_ia32_psubw((__v4hi)__m1, (__v4hi)__m2);
*/
#define SDL_DISABLE_IMMINTRIN_H 1
#endif

#include <SDL.h>
#include <SDL_image.h>

#ifdef __ANDROID__

#include <android/log.h>
#include <jni.h>

#define TAG "Boxed"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

JavaVM* gJavaVM;

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
  gJavaVM = vm;
  return JNI_VERSION_1_6;
}
#else
#define LOGE(...) printf(__VA_ARGS__)
#define LOGW(...) printf(__VA_ARGS__)
#define LOGI(...) printf(__VA_ARGS__)
#define LOGD(...) printf(__VA_ARGS__)
#endif

void onAudioDeviceCallback(void* userdata, Uint8* stream, int len);

Uint8 *hitBuffer;
Uint32 hitLength;

int main(int argc, char* argv[]) {
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    LOGE("Error: %s\n", SDL_GetError());
    return -1;
  }

  SDL_Window *window = SDL_CreateWindow("Boxer",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    640, 480,
    SDL_WINDOW_OPENGL);
  if (window == NULL) {
    LOGE("Error: %s\n", SDL_GetError());
    return -1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
  if (renderer == NULL) {
    LOGE("Error: %s\n", SDL_GetError());
    return -1;
  }

  int running = 1;
  SDL_Event event;

  SDL_Rect player;
  player.x = 50;
  player.y = 50;
  player.h = 50;
  player.w = 50;

  SDL_AudioSpec hit;
  if (SDL_LoadWAV("assets/hit.wav", &hit, &hitBuffer, &hitLength) == NULL) {
    LOGE("Error: %s\n", SDL_GetError());
    return -1;
  }
  hit.callback = onAudioDeviceCallback;
  SDL_AudioDeviceID device = SDL_OpenAudioDevice(NULL, 0, &hit, NULL, 0);
  if (device == 0) {
    LOGE("Error: %s\n", SDL_GetError());
    return -1;
  }

  // uncomment to play audio
  // SDL_PauseAudioDevice(device, 0);

  SDL_Texture *enemyTexture = IMG_LoadTexture(renderer, "assets/enemy.png");
  if (enemyTexture == NULL) {
    LOGE("Error: %s\n", SDL_GetError());
    return -1;
  }

  SDL_Rect enemy;
  enemy.x = 250;
  enemy.y = 250;
  enemy.h = 50;
  enemy.w = 50;

  SDL_Rect enemyPose;
  enemyPose.x = 0;
  enemyPose.y = 0;
  enemyPose.h = 16;
  enemyPose.w = 16;

  Uint64 nextEnemyPoseTimeout = SDL_GetTicks64() + 100;

  Uint64 viaCepTimeout = SDL_GetTicks64() + 3000;

  SDL_Rect httpCall;
  httpCall.x = 250;
  httpCall.y = 50;
  httpCall.h = 50;
  httpCall.w = 50;
  Uint8 httpCallRComp = 200;
  Uint8 httpCallGComp = 200;

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = 0;
      } else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
          case SDLK_DOWN: player.y += 10; break;
          case SDLK_UP: player.y -= 10; break;
          case SDLK_RIGHT: player.x += 10; break;
          case SDLK_LEFT: player.x -= 10; break;
          case SDLK_s: enemy.y += 10; break;
          case SDLK_w: enemy.y -= 10; break;
          case SDLK_d: enemy.x += 10; break;
          case SDLK_a: enemy.x -= 10; break;
          default: break;
        }
      }
    }
    SDL_SetRenderDrawColor(renderer, 0, 50, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 200, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &player);
    SDL_SetRenderDrawColor(renderer, httpCallRComp, httpCallGComp, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &httpCall);
    SDL_RenderCopy(renderer, enemyTexture, &enemyPose, &enemy);
    SDL_RenderPresent(renderer);
    if (SDL_GetTicks64() >= nextEnemyPoseTimeout) {
      enemyPose.x += 16;
      if (enemyPose.x == 256) {
        enemyPose.x = 0;
      }
      nextEnemyPoseTimeout = SDL_GetTicks64() + 100;
    }
    if (viaCepTimeout >= 0 && SDL_GetTicks64() >= viaCepTimeout) {
      viaCepTimeout = -1;

      #ifdef __ANDROID__
      JNIEnv *env;
      jint res = (*gJavaVM)->GetEnv(gJavaVM, (void**)&env, JNI_VERSION_1_6);
      LOGE("GetEnv return=%d (expected %d)\n", res, JNI_OK);

      jstring cep = (*env)->NewStringUTF(env, "14820464");
      jclass cls = (*env)->FindClass(env, "dev/samuel/boxed/HTTPRequests");
      jmethodID mid = (*env)->GetStaticMethodID(env, cls, "callViaCep", "(Ljava/lang/String;)Ljava/lang/String;");
      jstring retorno = (*env)->CallStaticObjectMethod(env, cls, mid, cep);

      const char* retornoChars = (*env)->GetStringUTFChars(env, retorno, NULL);
      LOGE("CallStatic return=%s\n", (const char *)retornoChars);
      (*env)->ReleaseStringUTFChars(env, retorno, retornoChars);
      #endif

      // CURL *curl = curl_easy_init();
      // if (curl == NULL) {
      //   httpCallRComp = 255;
      //   httpCallGComp = 0;
      // } else {

      //   CURLcode res;
      //   char errbuf[CURL_ERROR_SIZE];
      //   errbuf[0] = 0;

      //   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
      //   curl_easy_setopt(curl, CURLOPT_URL, "https://viacep.com.br/ws/14820464/json/");
      //   curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);

      //   res = curl_easy_perform(curl);

      //   if (res != CURLE_OK) {
      //     httpCallRComp = 10;
      //     httpCallGComp = 50;

      //     LOGE("\n\nError: %s\n", errbuf);

      //   } else {

      //     long responseCode;
      //     curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

      //     curl_easy_cleanup(curl);

      //     LOGE("\n\nResponse code: %ld\n", responseCode);
      //     if (responseCode >= 200 && responseCode < 300) {
      //       httpCallRComp = 0;
      //       httpCallGComp = 255;
      //     } else {
      //       httpCallRComp = 200;
      //       httpCallGComp = 50;
      //     }

      //   }

      // }

    }
  }

  SDL_FreeWAV(hitBuffer);
  SDL_Quit();

  return 0;
}

void onAudioDeviceCallback(void *userdata, Uint8 *stream, int len) {
  // awesome summary
  // https://gist.github.com/armornick/3447121
  printf("audio device called..\n");
  static Uint32 hitCurrentPos = 0;
  int effectiveLen = len > hitLength - hitCurrentPos ? hitLength - hitCurrentPos : len;
  SDL_memcpy(stream, hitBuffer + hitCurrentPos, effectiveLen);
  // use https://wiki.libsdl.org/SDL2/SDL_MixAudioFormat if you want mixing or another library entirely
  hitCurrentPos += len;
  if (hitCurrentPos > hitLength) {
    hitCurrentPos = 0;
  }
}
