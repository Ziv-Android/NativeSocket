LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := my-socket
LOCAL_SRC_FILES := common.cpp TcpTools.cpp UdpTools.cpp com_ziv_socket_echo.cpp
LOCAL_C_INCLUDE := $(LOCAL_PATH) $(LOCAL_PATH)/include

LOCAL_LDLIBS += -llog

include $(BUILD_SHARED_LIBRARY)