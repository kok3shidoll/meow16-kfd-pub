/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef thread_h
#define thread_h

#include <stdint.h>

struct thread {
    uint64_t task_threads_next;
    uint64_t task_threads_prev;
    uint64_t map;
    uint64_t thread_id;
    uint64_t object_size;
};

static const struct thread thread_versions[] = {
    // Note: sizes below here are wrong idc
    {
        .task_threads_next = 0x368,
        .task_threads_prev = 0x370,
        .map               = 0x380,
        .thread_id         = 0x3f0,
        .object_size       = 0x498
    }, // iOS 16.0 - 16.1 arm64
    
    {
        .task_threads_next = 0x368,
        .task_threads_prev = 0x370,
        .map               = 0x380,
        .thread_id         = 0x3f0,
        .object_size       = 0x498
    }, // iOS 16.2 - 16.3 arm64
    
    {
        .task_threads_next = 0x368,
        .task_threads_prev = 0x370,
        .map               = 0x380,
        .thread_id         = 0x3f0,
        .object_size       = 0x498
    }, // iOS 16.4 - 16.6 arm64
};

typedef uint64_t thread_task_threads_next_t;
typedef uint64_t thread_task_threads_prev_t;
typedef uint64_t thread_map_t;
typedef uint64_t thread_thread_id_t;

#endif /* thread_h */
