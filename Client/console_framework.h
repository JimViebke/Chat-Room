#pragma once

#include <console_framework.h>

#if defined(_DEBUG) && defined(_DLL)
#pragma comment (lib, "conlib-mt-gd.lib")
#elif defined(_DEBUG) && !defined(_DLL)
#pragma comment (lib, "conlib-mt-sgd.lib")
#elif !defined(_DEBUG) && defined(_DLL)
#pragma comment (lib, "conlib-mt.lib")
#elif !defined(_DEBUG) && !defined(_DLL)
#pragma comment (lib, "conlib-mt-s.lib")
#endif