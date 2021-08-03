#include <jni.h>
#include <android/log.h>
#include <stdlib.h>

static const char* LOG_TAG = "ScreenBackend";

#define UNUSED __attribute__((__unused__))
#define CHECK(cond) do { if(!(cond)) { __android_log_assert(NULL, LOG_TAG, "Check '%s' failed on screen.c line %d", #cond, __LINE__); abort(); } } while(0)

struct ScreenBackend {
    int width;
    int height;
};
typedef struct ScreenBackend ScreenBackend;

JNIEXPORT jlong JNICALL Java_com_termux_app_activities_ScreenBackend_init(
    UNUSED JNIEnv *env, UNUSED jclass clazz
) {
    ScreenBackend* backend = malloc(sizeof(ScreenBackend));
    CHECK(backend != NULL);

    backend->width = 1920;
    backend->height = 1080;

    return (jlong)backend;
}

JNIEXPORT void JNICALL Java_com_termux_app_activities_ScreenBackend_destroy(
    UNUSED JNIEnv *env, UNUSED jclass clazz, jlong backendHandle
) {
    ScreenBackend* backend = (ScreenBackend*)backendHandle;
    CHECK(backend != NULL);

    free(backend);
}

JNIEXPORT jint JNICALL Java_com_termux_app_activities_ScreenBackend_getWidth(
    UNUSED JNIEnv *env, UNUSED jclass clazz, jlong backendHandle
) {
    ScreenBackend* backend = (ScreenBackend*)backendHandle;
    CHECK(backend != NULL);

    return backend->width;
}

JNIEXPORT jint JNICALL Java_com_termux_app_activities_ScreenBackend_getHeight(
    UNUSED JNIEnv *env, UNUSED jclass clazz, jlong backendHandle
) {
    ScreenBackend* backend = (ScreenBackend*)backendHandle;
    CHECK(backend != NULL);

    return backend->height;
}
