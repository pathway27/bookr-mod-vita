#if defined(__vita__) && defined(DEBUG)
  #include <psp2/kernel/clib.h>
  #define printf sceClibPrintf
#else
  #define printf
#endif