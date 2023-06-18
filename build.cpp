#include "../casein/build.hpp"
#include "build.hpp"

int main(int argc, char **argv) {
  using namespace ecow;

  auto poc = unit::create<mod>("poc");
  poc->add_wsdep("casein", casein());

  auto all = unit::create<app>("poc");
  all->add_requirement(native);
  all->add_ref(quack());
  all->add_ref(poc);
  return run_main(all, argc, argv);
}
