#include <base/rust.h>

#include <cstdint>

#ifdef __clang__
#pragma clang diagnostic ignored "-Wdollar-in-identifier-extension"
#endif // __clang__

extern "C" {
::std::int32_t cxxbridge1$194$str_width_unicode(char const *text) noexcept;
} // extern "C"

::std::int32_t str_width_unicode(char const *text) noexcept
{
	return cxxbridge1$194$str_width_unicode(text);
}
