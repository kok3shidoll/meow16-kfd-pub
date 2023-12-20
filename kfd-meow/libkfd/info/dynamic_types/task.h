/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef task_h
#define task_h

#include <stdint.h>

struct task {
    uint64_t map;
    uint64_t threads_next;
    uint64_t threads_prev;
    uint64_t itk_space;
    uint64_t object_size;
};

static const struct task task_versions[] = {
    // Note: sizes below here are wrong idc
    {
        .map            = 0x28,
        .threads_next   = 0x58,
        .threads_prev   = 0x60,
        .itk_space      = 0x300,
        .object_size    = 0x640
    }, // iOS 16.0 - 16.1 arm64
    
    {
        .map            = 0x28,
        .threads_next   = 0x58,
        .threads_prev   = 0x60,
        .itk_space      = 0x300,
        .object_size    = 0x640
    }, // iOS 16.2 - 16.3 arm64
    
    {
        .map            = 0x28,
        .threads_next   = 0x58,
        .threads_prev   = 0x60,
        .itk_space      = 0x300,
        .object_size    = 0x640
    }, // iOS 16.4 - 16.6 arm64
};

typedef uint64_t task_map_t;
typedef uint64_t task_threads_next_t;
typedef uint64_t task_threads_prev_t;
typedef uint64_t task_itk_space_t;

#endif /* task_h */
