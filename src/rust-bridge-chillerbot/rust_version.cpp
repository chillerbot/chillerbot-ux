#include "base/rust.h"

extern "C" {
int cxxbridge1$str_width_unicode(const char *text) noexcept;
} // extern "C"

int str_width_unicode(const char *text) noexcept {
  return cxxbridge1$str_width_unicode(text);
}

