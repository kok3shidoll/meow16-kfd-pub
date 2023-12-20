/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef kqworkloop_h
#define kqworkloop_h

#include <stdint.h>

struct kqworkloop {
    uint64_t kqwl_state;
    uint64_t kqwl_p;
    uint64_t kqwl_owner;
    uint64_t kqwl_dynamicid;
    uint64_t object_size;
};

static const struct kqworkloop kqworkloop_versions[] = {
    
    // idk
    {
        .kqwl_state     = 0x10,
        .kqwl_p         = 0x18,
        .kqwl_owner     = 0xd0,
        .kqwl_dynamicid = 0xe8,
        .object_size    = 0x108
    }, // iOS 16.0 - 16.1 arm64
    
    {
        .kqwl_state     = 0x10,
        .kqwl_p         = 0x18,
        .kqwl_owner     = 0xd0,
        .kqwl_dynamicid = 0xe8,
        .object_size    = 0x108
    }, // iOS 16.2 - 16.3 arm64
    
    {
        .kqwl_state     = 0x10,
        .kqwl_p         = 0x18,
        .kqwl_owner     = 0xd0,
        .kqwl_dynamicid = 0xe8,
        .object_size    = 0x108
    }, // iOS 16.4 - 16.6 arm64
};

typedef uint16_t kqworkloop_kqwl_state_t;
typedef uint64_t kqworkloop_kqwl_p_t;
typedef uint64_t kqworkloop_kqwl_owner_t;
typedef uint64_t kqworkloop_kqwl_dynamicid_t;

#endif /* kqworkloop_h */
