#pragma once

#include <cassert>
#include "Defines.h"

/// Create a convenient logging macro. Android automatically adds newlines
/// iOS does not, so we normalize the output to append a newline if not yet
/// present.
///
/// It also defines
#ifdef ANDROID
#   include <android/log.h>
#   ifdef DEBUG
#       define LOG(...) __android_log_print(ANDROID_LOG_INFO, "gameoven", __VA_ARGS__);
#   else
#       define LOG(...) { }
#   endif
#else
#   ifdef DEBUG
#	define LOG(macro_format, ...) {                                            \
        printf(macro_format, ##__VA_ARGS__);                                   \
        const size_t macro_len = strlen(macro_format);                         \
        if(macro_len > 0 && macro_format[macro_len - 1] != '\n') {             \
            printf("\n");                                                      \
        }                                                                      \
    }
#   else
#       define LOG(...) { }
#   endif
#endif

/// IOS print in the on-device console
///
#ifndef ANDROID
#   ifdef DEBUG
#       define NSLOG(fmt, ...) NSLog(fmt, ##__VA_ARGS__)
#   else
#       define NSLOG(fmt, ...) {}
#   endif
#else
#	define NSLOG(fmt, ...) {}
#endif


/// A crossplatform debug assertion trigger. Android does not mention a filename
/// nor line number. This macro simply adds them.
///
#ifdef ANDROID
#   define ASSERT(assertion)                                                   \
        if(!assertion) {                                                       \
            LOG("Assertion triggered in %s on line %i", __FILE__, __LINE__);   \
            assert(assertion);                                                 \
        }
#else
#   define ASSERT(assertion) assert(assertion);
#endif

/// A friendly replacement for "assert(0)". Triggers a debug assertion and outputs
/// the friendly, programmer supplied, message.
///
#define ERROR(...) { LOG(__VA_ARGS__); ASSERT(0) };

#define _UNSUED(x) ((void)x)
