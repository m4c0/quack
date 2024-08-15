export module quack:daffy;
import :objects;
import jute;

namespace quack::daffy {
  export void app_name(jute::view n);
  export void add_batch(unsigned max, void (*fn)(quack::instance *&));
} // namespace quack::daffy
