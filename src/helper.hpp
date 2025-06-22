#pragma once

#ifdef __EMSCRIPTEN__

#include <emscripten.h>

EM_JS(bool, isMobile, (), {
    return /iPhone|iPad|iPod|Android/i.test(navigator.userAgent);
})
#endif

