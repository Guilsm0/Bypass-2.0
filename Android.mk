# Android.mk
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := bypass_ultimate
LOCAL_SRC_FILES := bypass_ultimate.cpp bypass_ultimate_loader.cpp
LOCAL_LDLIBS := -llog -ldl -lz
LOCAL_CFLAGS := -std=c++17 -O2 -fvisibility=hidden
LOCAL_CPPFLAGS := -frtti -fexceptions
include $(BUILD_SHARED_LIBRARY)