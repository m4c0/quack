export module quack;
export import :objects;
import casein;
import hai;

export namespace quack {
class pimpl;
class renderer {
  hai::uptr<pimpl> m_pimpl;

public:
  renderer(unsigned max_quads);
  ~renderer();

  void setup(casein::native_handle_t);
  void fill_pos(const filler<pos> &);
  void fill_colour(const filler<colour> &);
  void repaint(unsigned i_count);
  void quit();
};
} // namespace quack
