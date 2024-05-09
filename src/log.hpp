#ifndef LOG_H
#define LOG_H

#ifdef __ANDROID__
	#include <android/log.h>
	#define LOGERR(...) __android_log_print(ANDROID_LOG_ERROR, "com.sdl.game", __VA_ARGS__)
#else
	#define LOGERR(...) printf(__VA_ARGS__); fflush(stdout);
#endif

#endif