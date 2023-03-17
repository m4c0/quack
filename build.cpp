#include "../vee/build.hpp"
#include "ecow.hpp"

int main(int argc, char **argv) {
  using namespace ecow;
  auto poc = unit::create<app>("poc");
  return run_main(poc, argc, argv);
}
