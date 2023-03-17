#include "../casein/build.hpp"
#include "../hai/build.hpp"
#include "../vee/build.hpp"
#include "ecow.hpp"

int main(int argc, char **argv) {
  using namespace ecow;

  constexpr const auto setup = [](auto &mf) -> auto & {
    mf.add_wsdep("casein", casein());
    mf.add_wsdep("hai", hai());
    mf.add_part("objects");
    return mf;
  };

  auto m = unit::create<per_feat<mod>>("quack");
  setup(m->for_feature(android_ndk));
  setup(m->for_feature(posix));
  setup(m->for_feature(webassembly)).add_impl("wasm");

  auto poc = unit::create<app>("poc");
  poc->add_wsdep("casein", casein());
  poc->add_ref(m);
  poc->add_unit("poc");
  return run_main(poc, argc, argv);
}
