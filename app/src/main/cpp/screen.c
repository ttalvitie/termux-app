#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <jni.h>
#include <stdlib.h>

static const char* LOG_TAG = "ScreenBackend";

#define UNUSED __attribute__((__unused__))
#define CHECK(cond) do { if(!(cond)) { __android_log_assert(NULL, LOG_TAG, "Check '%s' failed on screen.c line %d", #cond, __LINE__); abort(); } } while(0)

#define LOG __android_log_print
#define LEVEL_DEBUG ANDROID_LOG_DEBUG, LOG_TAG

struct ScreenBackend {
    int width;
    int height;
    ANativeWindow* surface;
};
typedef struct ScreenBackend ScreenBackend;

static void ScreenBackend_stopRender(ScreenBackend* backend);

static ScreenBackend* ScreenBackend_init() {
    ScreenBackend* backend = malloc(sizeof(ScreenBackend));
    CHECK(backend != NULL);

    LOG(LEVEL_DEBUG, "Initializing backend");

    backend->width = 1920;
    backend->height = 1080;
    backend->surface = NULL;

    return backend;
}

static void ScreenBackend_destroy(ScreenBackend* backend) {
    CHECK(backend != NULL);

    ScreenBackend_stopRender(backend);

    LOG(LEVEL_DEBUG, "Destroying backend");
    free(backend);
}

static int ScreenBackend_getWidth(ScreenBackend* backend) {
    CHECK(backend != NULL);
    return backend->width;
}

static int ScreenBackend_getHeight(ScreenBackend* backend) {
    CHECK(backend != NULL);
    return backend->height;
}

static void ScreenBackend_startRender(ScreenBackend* backend, ANativeWindow* surface) {
    CHECK(backend != NULL);
    CHECK(surface != NULL);

    ScreenBackend_stopRender(backend);

    LOG(LEVEL_DEBUG, "Starting renderer");
    CHECK(backend->surface == NULL);
    backend->surface = surface;
}

static void ScreenBackend_stopRender(ScreenBackend* backend) {
    CHECK(backend != NULL);

    if(backend->surface != NULL) {
        LOG(LEVEL_DEBUG, "Stopping renderer");
        ANativeWindow_release(backend->surface);
        backend->surface = NULL;
    }
}

JNIEXPORT jlong JNICALL Java_com_termux_app_activities_ScreenBackend_init(
    UNUSED JNIEnv *env, UNUSED jclass clazz
) {
    return (jlong)ScreenBackend_init();
}

JNIEXPORT void JNICALL Java_com_termux_app_activities_ScreenBackend_destroy(
    UNUSED JNIEnv *env, UNUSED jclass clazz, jlong backend
) {
    ScreenBackend_destroy((ScreenBackend*)backend);
}

JNIEXPORT jint JNICALL Java_com_termux_app_activities_ScreenBackend_getWidth(
    UNUSED JNIEnv *env, UNUSED jclass clazz, jlong backend
) {
    return (jint)ScreenBackend_getWidth((ScreenBackend*)backend);
}

JNIEXPORT jint JNICALL Java_com_termux_app_activities_ScreenBackend_getHeight(
    UNUSED JNIEnv *env, UNUSED jclass clazz, jlong backend
) {
    return (jint)ScreenBackend_getHeight((ScreenBackend*)backend);
}

JNIEXPORT void JNICALL Java_com_termux_app_activities_ScreenBackend_startRender(
    JNIEnv *env, UNUSED jclass clazz, jlong backend, jobject surface
) {
    ScreenBackend_startRender((ScreenBackend*)backend, ANativeWindow_fromSurface(env, surface));
}

JNIEXPORT void JNICALL Java_com_termux_app_activities_ScreenBackend_stopRender(
    UNUSED JNIEnv *env, UNUSED jclass clazz, jlong backend
) {
    ScreenBackend_stopRender((ScreenBackend*)backend);
}
