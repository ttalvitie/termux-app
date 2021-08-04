#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <errno.h>
#include <jni.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

static const char* LOG_TAG = "ScreenBackend";

#define UNUSED __attribute__((__unused__))
#define CHECK(cond) do { if(!(cond)) { __android_log_assert(NULL, LOG_TAG, "Check '%s' failed on screen.c line %d", #cond, __LINE__); abort(); } } while(0)

#define LOG __android_log_print
#define LEVEL_DEBUG ANDROID_LOG_DEBUG, LOG_TAG

struct ScreenBackend {
    int width;
    int height;
    ANativeWindow* surface;
    pthread_t renderThread;
    int renderThreadRunFlag;
    pthread_mutex_t mutex;
    pthread_condattr_t cvAttr;
    pthread_cond_t cv;
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
    backend->renderThreadRunFlag = 0;
    CHECK(pthread_mutex_init(&backend->mutex, NULL) == 0);
    CHECK(pthread_condattr_init(&backend->cvAttr) == 0);
    CHECK(pthread_condattr_setclock(&backend->cvAttr, CLOCK_MONOTONIC) == 0);
    CHECK(pthread_cond_init(&backend->cv, &backend->cvAttr) == 0);

    return backend;
}

static void ScreenBackend_destroy(ScreenBackend* backend) {
    CHECK(backend != NULL);

    ScreenBackend_stopRender(backend);

    LOG(LEVEL_DEBUG, "Destroying backend");

    CHECK(pthread_condattr_destroy(&backend->cvAttr) == 0);
    CHECK(pthread_cond_destroy(&backend->cv) == 0);
    CHECK(pthread_mutex_destroy(&backend->mutex) == 0);
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

static void* renderThreadImpl(void* data) {
    ScreenBackend* backend = (ScreenBackend*)data;
    CHECK(backend != NULL);

    CHECK(pthread_mutex_lock(&backend->mutex) == 0);
    while(backend->renderThreadRunFlag) {
        {
            CHECK(pthread_mutex_unlock(&backend->mutex) == 0);

            ANativeWindow_Buffer buffer;
            CHECK(ANativeWindow_lock(backend->surface, &buffer, NULL) == (uint32_t)0);

            CHECK(buffer.format == AHARDWAREBUFFER_FORMAT_R8G8B8X8_UNORM);
            for(int32_t y = 0; y < buffer.height; ++y) {
                uint8_t* pos = (uint8_t*)buffer.bits + 4 * buffer.stride * y;
                for(int32_t x = 0; x < buffer.width; ++x) {
                    uint8_t val = (uint8_t)x ^ (uint8_t)y;
                    for(int i = 0; i < 4; ++i) {
                        *pos++ = val;
                    }
                }
            }

            CHECK(ANativeWindow_unlockAndPost(backend->surface) == (uint32_t)0);

            CHECK(pthread_mutex_lock(&backend->mutex) == 0);
        }

        struct timespec timeout;
        CHECK(clock_gettime(CLOCK_MONOTONIC, &timeout) == 0);
        /* Wait at most 10ms as we may get more screen data to render. */
        timeout.tv_nsec += 10000000l;
        if(timeout.tv_nsec >= 1000000000l) {
            timeout.tv_nsec -= 1000000000l;
            ++timeout.tv_sec;
        }

        int waitResult = pthread_cond_timedwait(&backend->cv, &backend->mutex, &timeout);
        CHECK(waitResult == 0 || waitResult == ETIMEDOUT);
    }
    CHECK(pthread_mutex_unlock(&backend->mutex) == 0);

    return NULL;
}

static void ScreenBackend_startRender(ScreenBackend* backend, ANativeWindow* surface) {
    CHECK(backend != NULL);
    CHECK(surface != NULL);

    ScreenBackend_stopRender(backend);

    LOG(LEVEL_DEBUG, "Starting render thread");

    CHECK(backend->surface == NULL);
    backend->surface = surface;
    backend->renderThreadRunFlag = 1;
    CHECK(pthread_create(&backend->renderThread, NULL, renderThreadImpl, (void*)backend) == 0);
}

static void ScreenBackend_stopRender(ScreenBackend* backend) {
    CHECK(backend != NULL);

    if(backend->surface != NULL) {
        LOG(LEVEL_DEBUG, "Stopping render thread");

        CHECK(pthread_mutex_lock(&backend->mutex) == 0);
        backend->renderThreadRunFlag = 0;
        CHECK(pthread_mutex_unlock(&backend->mutex) == 0);
        CHECK(pthread_cond_signal(&backend->cv) == 0);

        CHECK(pthread_join(backend->renderThread, NULL) == 0);

        ANativeWindow_release(backend->surface);
        backend->surface = NULL;

        LOG(LEVEL_DEBUG, "Render thread stopped successfully");
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
