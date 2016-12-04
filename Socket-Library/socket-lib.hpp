#if !defined(GUARD_SOCKLIB_HEADER)
#define GUARD_SOCKLIB_HEADER

#if defined(_DEBUG) && defined(_DLL)
#pragma comment (lib, "socklib-mt-gd.lib")
#elif defined(_DEBUG) && !defined(_DLL)
#pragma comment (lib, "socklib-mt-sgd.lib")
#elif !defined(_DEBUG) && defined(_DLL)
#pragma comment (lib, "socklib-mt.lib")
#elif !defined(_DEBUG) && !defined(_DLL)
#pragma comment (lib, "socklib-mt-s.lib")
#endif

#endif // GUARD_SOCKLIB_HEADER