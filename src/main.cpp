#include "bookr.hpp"

// WHERE TO PUT THIS?!
int _newlib_heap_size_user = 192 * 1024 * 1024;

// extern "C"
int main(int argc, char *argv[]) {
  bookr::initalise(argc, argv);
  // bookr::mainloop();
  // bookr::cleanup();
  return 0;
}