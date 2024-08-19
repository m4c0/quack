export module quack:daffy;
import :objects;
import dotz;
import jute;

/// Attempt at a multi-batch donald
namespace quack::daffy {
  export void app_name(jute::view n);
  export void clear_colour(dotz::vec4 c);

  export void add_batch(unsigned max, unsigned img, void (*fn)(quack::instance *&));
  export void add_batch(unsigned max, unsigned img, void (*fn)(quack::instance *&, unsigned));

  export void add_image(jute::view name);
} // namespace quack::daffy
