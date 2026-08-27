/*
 * Copyright 2025-2026 Dair Aidarkhanov
 * SPDX-License-Identifier: Zlib
 */

#ifndef UWIDTH_H
#define UWIDTH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UWIDTH_VERSION_MAJOR 1
#define UWIDTH_VERSION_MINOR 0
#define UWIDTH_VERSION_PATCH 0

#define UWIDTH_UNICODE_VERSION_MAJOR 18
#define UWIDTH_UNICODE_VERSION_MINOR 0
#define UWIDTH_UNICODE_VERSION_PATCH 0
#define UWIDTH_UNICODE_VERSION_IS_BETA 1

typedef uint_least32_t Uwidth_Code_Point;
typedef uint_least32_t Uwidth_Profile;
typedef uint_least32_t Uwidth_Event_Kind;
typedef uint_least32_t Uwidth_State_Word;

/*
 * East Asian Ambiguous characters have width 1 under the narrow profile and
 * width 2 under the East Asian profile.
 */
enum {
    uwidth_profile_narrow,
    uwidth_profile_east_asian
};

enum {
    uwidth_event_none,
    uwidth_event_cluster,
    uwidth_event_control
};

/* A CRLF pair produces one control event containing U+000D. */
typedef union Uwidth_Event {
    uint_least32_t width;
    Uwidth_Code_Point control;
} Uwidth_Event;

/*
 * Opaque stream state. The word fields are private. A copied state can be
 * continued independently of the original.
 */
typedef struct Uwidth_State {
    Uwidth_State_Word words[2];
} Uwidth_State;

void uwidth_init(
    Uwidth_State *state,
    Uwidth_Profile profile
);

/*
 * Feed the next code point. Returns uwidth_event_none while it continues the
 * open cluster. When it starts a new cluster, returns uwidth_event_cluster or
 * uwidth_event_control for the previous cluster. Surrogates and values above
 * U+10FFFF are treated as U+FFFD.
 */
Uwidth_Event_Kind uwidth_push(
    Uwidth_State *state,
    Uwidth_Code_Point code_point,
    Uwidth_Event *event
);

/* Emit any pending cluster or control, then clear the stream. */
Uwidth_Event_Kind uwidth_finish(
    Uwidth_State *state,
    Uwidth_Event *event
);

/* Drop pending input without changing the selected profile. */
void uwidth_reset(Uwidth_State *state);

#ifdef __cplusplus
}
#endif

#endif
