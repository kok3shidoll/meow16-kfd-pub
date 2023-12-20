/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef proc_h
#define proc_h

#include <stdint.h>

struct proc {
    uint64_t p_list_le_next;
    uint64_t p_list_le_prev;
    uint64_t task;
    uint64_t p_pid;
    uint64_t p_fd_fd_ofiles;
    uint64_t object_size;
};

static const struct proc proc_versions[] = {
    // Note: sizes below here are wrong idc
    {
        .p_list_le_next = 0x0,
        .p_list_le_prev = 0x8,
        .task           = 0,
        .p_pid          = 0x60,
        .p_fd_fd_ofiles = 0xf8,
        .object_size    = 0x530
    }, // iOS 16.0 - 16.1 arm64
    
    {
        .p_list_le_next = 0x0,
        .p_list_le_prev = 0x8,
        .task           = 0,
        .p_pid          = 0x60,
        .p_fd_fd_ofiles = 0xf8,
        .object_size    = 0x538
    }, // iOS 16.2 - 16.3 arm64
    
    {
        .p_list_le_next = 0x0,
        .p_list_le_prev = 0x8,
        .task           = 0,
        .p_pid          = 0x60,
        .p_fd_fd_ofiles = 0xf8,
        .object_size    = 0x730
    }, // iOS 16.4 - 16.6 arm64
};

typedef uint64_t proc_p_list_le_next_t;
typedef uint64_t proc_p_list_le_prev_t;
typedef uint64_t proc_task_t;
typedef int32_t proc_p_pid_t;
typedef uint64_t proc_p_fd_fd_ofiles_t;

#endif /* proc_h */
