LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := AlertBuddy
LOCAL_SRC_FILES := AlertBuddy.c

include ${BUILD_SHARED_LIBRARY}