LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
OPENCV_CAMERA_MODULES:=on
OPENCV_INSTALL_MODULES:=on
OPENCV_LIB_TYPE:=SHARED
include opencv310sdk\native\jni\OpenCV.mk
LOCAL_MODULE    := ofo
LOCAL_SRC_FILES := ofo.cpp
LOCAL_LDLIBS := -llog
include $(BUILD_SHARED_LIBRARY)
