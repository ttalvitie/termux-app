LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libtermux-bootstrap
LOCAL_SRC_FILES := termux-bootstrap-zip.S termux-bootstrap.c
include $(BUILD_SHARED_LIBRARY)
include $(CLEAR_VARS)
LOCAL_MODULE := libtermux-screen
LOCAL_SRC_FILES := screen.c
LOCAL_LDLIBS := -landroid -llog
include $(BUILD_SHARED_LIBRARY)
