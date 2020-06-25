#include "bookr.hpp"

int main(int argc, char *argv[]) {
  bookr::initalise(argc, argv);
  bookr::mainloop();
  bookr::cleanup();
  return 0;
}