#include "../casein/build.hpp"
#include "build.hpp"

int main(int argc, char **argv) {
  using namespace ecow;

  auto poc = unit::create<app>("poc");
  poc->add_wsdep("casein", casein());
  poc->add_ref(quack());
  poc->add_unit("poc");
  return run_main(poc, argc, argv);
}
