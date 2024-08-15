module quack;
import traits;

static constexpr const unsigned instance_size = sizeof(quack::instance);

quack::buffer_updater::buffer_updater(voo::device_and_queue * dq, unsigned max_quads, void (*fn)(instance *&))
    : updater { dq->queue(), voo::h2l_buffer { dq->physical_device(), max_quads * instance_size } }
    , m_fn { fn } {
  run_once();
}

void quack::buffer_updater::update_data(voo::h2l_buffer * buf) {
  voo::mapmem m { buf->host_memory() };
  auto mb = static_cast<instance *>(*m);
  auto me = mb;
  m_fn(me);
  if (me >= mb) m_inst_count = me - mb;
}

quack::image_updater::image_updater(voo::device_and_queue * dq, pipeline_stuff * ps, decltype(m_fn) fn)
    : updater { dq->queue(), {} }
    , m_fn { fn }
    , m_pd { dq->physical_device() }
    , m_dset { ps->allocate_descriptor_set() }
    , m_dset_old { ps->allocate_descriptor_set() } {
  run_once();
}

void quack::image_updater::update_data(voo::h2l_image * img) {
  m_old = traits::move(*img);

  *img = m_fn(m_pd);

  auto tmp = m_dset_old;
  m_dset_old = m_dset;
  m_dset = tmp;

  vee::update_descriptor_set(m_dset, 0, img->iv(), *m_smp);
}
