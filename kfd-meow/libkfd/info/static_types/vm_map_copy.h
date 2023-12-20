/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef vm_map_copy_h
#define vm_map_copy_h

#include "vm_map_entry.h"

#define cpy_hdr       c_u.hdr
#define cpy_object    c_u.object
#define cpy_kdata     c_u.kdata

struct rb_head {
    uint64_t rbh_root;
};

struct vm_map_header {
    struct vm_map_links links;
    int32_t nentries;
    uint16_t page_shift;
    uint32_t
        entries_pageable:1,
        __padding:15;
    struct rb_head rb_head_store;
};

struct vm_map_copy {
    int32_t type;
    uint64_t offset;
    uint64_t size;
    union {
        struct vm_map_header hdr;
        uint64_t object;
        uint64_t kdata;
    } c_u;
};

#endif /* vm_map_copy_h */
