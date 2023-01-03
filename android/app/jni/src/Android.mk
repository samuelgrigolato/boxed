LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

LOCAL_SRC_FILES := common/main.c

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image png16

LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)

$(call import-module,prefab/SDL2)
$(call import-module,prefab/SDL2_image)
$(call import-module,prefab/png16)
