LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	IDudiLowManagerService.cpp \
	DudiLowManagerService.cpp

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	libbinder 

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := libdudilowmanagerservice

include $(BUILD_SHARED_LIBRARY)


