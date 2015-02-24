LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	main_serverstart.cpp

LOCAL_SHARED_LIBRARIES := \
	libdudilowmanagerservice \
	libutils \
	libbinder

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../dudilowmanagerservice

LOCAL_MODULE := dudiserver

include $(BUILD_EXECUTABLE)
 
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	main_clientstart.cpp

LOCAL_SHARED_LIBRARIES := \
	libdudilowmanagerservice \
	libutils \
	libbinder

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../dudilowmanagerservice

LOCAL_MODULE := dudiclient

include $(BUILD_EXECUTABLE)
