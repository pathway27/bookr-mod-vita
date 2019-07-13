#include "bookr.hpp"

// WHERE TO PUT THIS?! Only for Vita newlib?
#ifdef __vita__
extern int _newlib_heap_size_user = 192 * 1024 * 1024;
#endif

// extern "C"
int main(int argc, char *argv[]) {
  bookr::initalise(argc, argv);
  bookr::mainloop();
  bookr::cleanup();
  return 0;
}