#include "../hai/build.hpp"
#include "../traits/build.hpp"
#include "../vee/build.hpp"
#include "ecow.hpp"

auto quack() {
  using namespace ecow;

  const auto setup = [](mod & mf) -> auto & {
    mf.add_wsdep("casein", casein());
    mf.add_wsdep("hai", hai());
    mf.add_part("objects");

    mf.add_unit<spirv>("quack.vert");
    mf.add_unit<spirv>("quack.frag");
    mf.add_resource("quack.vert.spv");
    mf.add_resource("quack.frag.spv");
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
    mf.add_part("v_stage");
    mf.add_part("v_pipeline");
    mf.add_impl("vulkan");
  };
  const auto setup_wasm = [&](mod &mf) {
    setup(mf);
    mf.add_impl("wasm");
    mf.add_feat<inline_js>("quack_fill_colour", R"((r, g, b) => {
  var rr = Math.pow(r, 1.0 / 2.2) * 256.0;
  var gg = Math.pow(g, 1.0 / 2.2) * 256.0;
  var bb = Math.pow(b, 1.0 / 2.2) * 256.0;
  ctx.fillStyle = `rgb(${rr}, ${gg}, ${bb})`;
})");
    mf.add_feat<inline_js>("quack_fill_rect", "ctx.fillRect.bind(ctx)");
  };

  auto m = unit::create<per_feat<mod>>("quack");
  setup_vulkan(m->for_feature(android_ndk));
  setup_vulkan(m->for_feature(posix));
  setup_wasm(m->for_feature(webassembly));
  return m;
}
