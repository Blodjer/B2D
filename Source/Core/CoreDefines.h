#pragma once

#define B2D

#define INLINE inline
#define FORCEINLINE __forceinline

#ifdef B2D_BUILD_DEBUG
    #define B2D_DEBUG_MEMORY
#endif

#ifdef B2D_BUILD_RELEASE
    #define B2D_NO_LOGGING
#endif

#ifndef B2D_BUILD_RELEASE
    #define B2D_EDITOR
#endif