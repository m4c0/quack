export module quack:daffy;
import :objects;
import jute;

namespace quack::daffy {
  export void app_name(jute::view n);

  export void add_batch(unsigned max, unsigned img, void (*fn)(quack::instance *&));
  export void add_batch(unsigned max, unsigned img, void (*fn)(quack::instance *&, unsigned));

  export void add_image(jute::view name);
} // namespace quack::daffy
