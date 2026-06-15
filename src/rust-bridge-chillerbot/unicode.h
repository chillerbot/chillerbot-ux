// This file can be included several times.
// ^
// hack for include guard CI check script

#pragma once
#include <base/rust.h>

#include <cstdint>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdollar-in-identifier-extension"
#endif // __clang__

::std::int32_t str_width_unicode(char const *text) noexcept;

#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__
