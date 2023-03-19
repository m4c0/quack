#include "../casein/build.hpp"
#include "../hai/build.hpp"
#include "../traits/build.hpp"
#include "../vee/build.hpp"
#include "ecow.hpp"

int main(int argc, char **argv) {
  using namespace ecow;

  const auto setup = [](auto &mf) -> auto & {
    mf.add_wsdep("casein", casein());
    mf.add_wsdep("hai", hai());
    mf.add_part("objects");
    return mf;
  };
  const auto setup_vulkan = [&](auto &mf) {
    setup(mf);
    mf.add_wsdep("traits", traits());
    mf.add_wsdep("vee", vee());
    mf.add_part("v_per_device");
    mf.add_part("v_per_extent");
    mf.add_part("v_per_frame");
    mf.add_part("v_per_inflight");
    mf.add_part("v_bbuffer");
    mf.add_part("v_pipeline");
    mf.add_impl("vulkan");
  };

  auto m = unit::create<per_feat<mod>>("quack");
  setup_vulkan(m->for_feature(android_ndk));
  setup_vulkan(m->for_feature(posix));
  setup(m->for_feature(webassembly)).add_impl("wasm");

  auto poc = unit::create<app>("poc");
  poc->add_wsdep("casein", casein());
  poc->add_ref(m);
  poc->add_unit("poc");
  return run_main(poc, argc, argv);
}
