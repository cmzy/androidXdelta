#ifndef __LOG_H__
#define __LOG_H__

#include <android/log.h>

#define LOG_VERBOSE 1
#define LOG_DEBUG 2
#define LOG_INFO 3
#define LOG_WARNING 4
#define LOG_ERROR 5
#define LOG_FATAL 6
#define LOG_SILENT 7

#ifndef MY_LOG_LEVEL
#define MY_LOG_LEVEL LOG_SILENT
#endif

///是否打开日志开关
#ifndef DEBUG
#define DEBUG MY_LOG_LEVEL
#endif

//默认的日志tag
#ifndef TAG
#define TAG (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#if(DEBUG <= LOG_VERBOSE)
#define LOGV(tag,...) __android_log_print(ANDROID_LOG_VERBOSE,tag,##__VA_ARGS__)
#else
#define LOGV(tag,...)
#endif

#if(DEBUG <= LOG_DEBUG)
#define LOGD(tag,...) __android_log_print(ANDROID_LOG_DEBUG,tag,##__VA_ARGS__)
#else
#define LOGD(tag,...)
#endif

#if(DEBUG <= LOG_INFO)
#define LOGI(tag,...) __android_log_print(ANDROID_LOG_INFO,tag,##__VA_ARGS__)
#else
#define LOGI(tag,...)
#endif

#if(DEBUG <= LOG_WARN)
#define LOGW(tag,...) __android_log_print(ANDROID_LOG_WARN,tag,##__VA_ARGS__)
#else
#define LOGW(tag,...)
#endif

#if(DEBUG  <= LOG_ERROR)
#define LOGE(tag,...) __android_log_print(ANDROID_LOG_ERROR,tag,##__VA_ARGS__)
#else
#define LOGE(tag,...)
#endif

#if(DEBUG <= LOG_FATAL)
#define LOGF(tag,...) __android_log_print(ANDROID_LOG_FATAL,tag,##__VA_ARGS__)
#else
#define LOGF(tag,...)
#endif

#endif /* LOG_H_ */

