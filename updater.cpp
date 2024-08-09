module quack;

static constexpr const unsigned instance_size = sizeof(quack::instance);

quack::buffer_updater::buffer_updater(voo::device_and_queue * dq, unsigned max_quads, void (*fn)(instance *))
    : updater { dq->queue(), voo::h2l_buffer { dq->physical_device(), max_quads * instance_size } }
    , m_fn { fn } {
  run_once();
}

void quack::buffer_updater::update_data(voo::h2l_buffer * buf) {
  voo::mapmem m { buf->host_memory() };
  m_fn(static_cast<instance *>(*m));
}
