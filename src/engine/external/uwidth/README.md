<picture>
  <source media="(prefers-color-scheme: dark)" srcset="assets/logo-light.svg">
  <img src="assets/logo.svg" alt="uwidth logo" width="64" height="64">
</picture>

`uwidth` measures the display width of a stream of Unicode code points, one
extended grapheme cluster at a time. It is written in C89.

> [!NOTE]
> The original `unicode-width` API is on the [`v0`](../../tree/v0) branch.

> [!WARNING]
> Preview releases ship tables built from the beta data for
> [Unicode 18.0.0](https://www.unicode.org/versions/beta-18.0.0.html). Unicode 18
> is not final yet, so those tables may change before `v1.0.0`.

## Using the library

Add `uwidth.h` and `uwidth.c` to your project, then pick the width profile that
matches your display environment:

- `uwidth_profile_narrow`: East Asian Ambiguous characters have width 1
- `uwidth_profile_east_asian`: East Asian Ambiguous characters have width 2

Call `uwidth_init`, feed each code point with `uwidth_push`, then call
`uwidth_finish` for anything still pending.

`uwidth_push` returns `uwidth_event_none` while the code point continues the
open cluster. When the code point starts a new cluster, it returns
`uwidth_event_cluster` or `uwidth_event_control` for the **previous** cluster.
Surrogates and values above U+10FFFF are treated as U+FFFD. Noncharacters are
accepted as ordinary scalars.

`uwidth_reset` drops pending input without changing the profile. A copied
`Uwidth_State` can be continued independently. A CRLF pair is one control event
containing CR.

Example: sum cluster widths and ignore controls.

```c
#include "uwidth.h"

static unsigned long measure(const Uwidth_Code_Point *text, unsigned int length)
{
    Uwidth_State state;
    Uwidth_Event event;
    unsigned long width = 0UL;
    unsigned int index;

    uwidth_init(&state, uwidth_profile_narrow);
    for (index = 0U; index < length; ++index) {
        if (uwidth_push(&state, text[index], &event)
            == uwidth_event_cluster) {
            width += event.width;
        }
    }
    if (uwidth_finish(&state, &event) == uwidth_event_cluster) {
        width += event.width;
    }
    return width;
}
```

## Unicode behavior

Grapheme boundaries follow
[UAX #29](https://www.unicode.org/reports/tr29/tr29-48.html), canonical
decomposition follows [UAX #15](https://www.unicode.org/reports/tr15/), and
ordinary width follows [UAX #11](https://www.unicode.org/reports/tr11/).
Emoji and text presentation use the sequences defined by
[UTS #51](https://www.unicode.org/reports/tr51/).

## Generating and testing

Regenerating or checking the tables needs Python 3.9 or later. The generator
reads the Unicode version macros from `uwidth.h`.

```sh
python3 generate.py
python3 generate.py --check
```

Build and run the tests:

```sh
cc -std=c89 -Wall -Wextra -Werror -O2 \
    uwidth.c uwidth_test.c -o uwidth_test
./uwidth_test
```

## License

uwidth combines code licensed under the
[zlib License](https://www.zlib.net/zlib_license.html) with tables derived from
Unicode data licensed under the
[Unicode License v3](https://www.unicode.org/license.txt).
