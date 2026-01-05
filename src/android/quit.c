#include "Connect4/android/quit.h"
#include <SDL3/SDL.h>

#ifdef SDL_PLATFORM_ANDROID
    #include <jni.h>
#endif

void Android_QuitAndRemoveTask(void) {
#ifdef SDL_PLATFORM_ANDROID
    JNIEnv *env = (JNIEnv *)SDL_GetAndroidJNIEnv();
    
    jobject activity = (jobject)SDL_GetAndroidActivity();
    if (!env || !activity) {
        return;
    }

    jclass cls = (*env)->GetObjectClass(env, activity);

    jmethodID mid = (*env)->GetStaticMethodID(env, cls, "quitAndRemoveTask", "()V");

    if (mid) {
        (*env)->CallStaticVoidMethod(env, cls, mid);
    }

    // Clean up local reference to class
    (*env)->DeleteLocalRef(env, cls);
#endif
}