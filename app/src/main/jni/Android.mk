LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := my-socket
LOCAL_SRC_FILES := echo.cpp
LOCAL_C_INCLUDE := $(LOCAL_PATH)/include

LOCAL_LDLIBS += -llog

include $(BUILD_SHARED_LIBRARY)