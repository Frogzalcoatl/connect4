#include "Connect4/android/quit.h"
#include <SDL3/SDL.h>

#if SDL_PLATFORM_ANDROID
    #include <jni.h>
#endif

void Android_QuitTask(bool removeTask) {
#if SDL_PLATFORM_ANDROID
    JNIEnv *env = (JNIEnv *)SDL_GetAndroidJNIEnv();
    if (!env) {
        return;
    }

    jclass cls = (*env)->FindClass(env, "org/libsdl/app/MyGameActivity");

    jmethodID mid = (*env)->GetStaticMethodID(env, cls, "quitTask", "(Z)V");

    if (mid) {
        (*env)->CallStaticVoidMethod(env, cls, mid, (jboolean)removeTask);
    }

    // Clean up local reference to class
    (*env)->DeleteLocalRef(env, cls);
#endif
}