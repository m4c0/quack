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
  void update(const filler &);
  void repaint();
  void quit();
};
} // namespace quack