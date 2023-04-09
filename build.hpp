#include "../hai/build.hpp"
#include "../missingno/build.hpp"
#include "../traits/build.hpp"
#include "../vee/build.hpp"
#include "ecow.hpp"

auto quack() {
  using namespace ecow;

  auto m = unit::create<mod>("quack");
  m->add_wsdep("casein", casein());
  m->add_wsdep("hai", hai());
  m->add_wsdep("missingno", missingno());
  m->add_wsdep("traits", traits());
  m->add_wsdep("vee", vee());

  m->add_part("objects");
  m->add_part("renderer");
  m->add_part("v_per_device");
  m->add_part("v_per_extent");
  m->add_part("v_per_frame");
  m->add_part("v_per_inflight");
  m->add_part("v_bbuffer");
  m->add_part("v_stage");
  m->add_part("v_pipeline_stuff");
  m->add_part("v_pipeline");
  m->add_impl("vulkan");

  m->add_unit<spirv>("quack.vert");
  m->add_unit<spirv>("quack.frag");
  m->add_resource("quack.vert.spv");
  m->add_resource("quack.frag.spv");

  return m;
}
