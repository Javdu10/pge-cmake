
#if defined (__ANDROID__)

#include "android/pch.h"

#endif

#if defined (__APPLE__)

#include "ios/ios_native_app_glue.h"
#include <memory>

#endif

#define OLC_PGE_APPLICATION
#if defined (__ANDROID__) or defined (__APPLE__)
#include "olcPixelGameEngine_Mobile.h"
#else
#include "olcPixelGameEngine.h"
#endif


