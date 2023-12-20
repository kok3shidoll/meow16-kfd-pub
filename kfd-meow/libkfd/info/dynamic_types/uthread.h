/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef uthread_h
#define uthread_h

#include <stdint.h>

struct uthread {
    uint64_t object_size;
};

static const struct uthread uthread_versions[] = {
    // Note: sizes below here are wrong idc
    { .object_size = 0x1b0 }, // iOS 16.0 - 16.1 arm64
    { .object_size = 0x1b0 }, // iOS 16.2 - 16.3 arm64
    { .object_size = 0x1b0 }, // iOS 16.4 - 16.6 arm64
};

#endif /* uthread_h */
