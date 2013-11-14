LOCAL_PATH := $(call my-dir)
cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-all -x $1
include $(CLEAR_VARS)

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    LOCAL_CFLAGS := -DHAVE_NEON=1
endif

# Define the default logging level based build type
ifeq ($(APP_OPTIM),release)
	MY_LOG_LEVEL := LOG_SILENT
else
	MY_LOG_LEVEL := LOG_VERBOSE
endif
MY_LOG_LEVEL := LOG_VERBOSE
LOCAL_CFLAGS += -DMY_LOG_LEVEL=$(MY_LOG_LEVEL)
LOCAL_CFLAGS += -fvisibility=hidden

LOCAL_LDLIBS += -L$(LOCAL_PATH) -llog

LOCAL_MODULE    := Patch
LOCAL_SRC_FILES := help/JNIHelp.h\
				   help/JNIHelp.cpp\
				   help/log.h\
				   Patch.h\
				   Patch.cpp\
				   Core.cpp

include $(BUILD_SHARED_LIBRARY)
