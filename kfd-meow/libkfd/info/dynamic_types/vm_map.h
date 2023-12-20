/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef vm_map_h
#define vm_map_h

#include "../static_types/vm_map_copy.h"

struct vm_map {
    uint64_t hdr_links_prev;
    uint64_t hdr_links_next;
    uint64_t min_offset;
    uint64_t max_offset;
    uint64_t hdr_nentries;
    uint64_t hdr_nentries_u64;
    uint64_t hdr_rb_head_store_rbh_root;
    uint64_t pmap;
    uint64_t hint;
    uint64_t hole_hint;
    uint64_t holes_list;
    uint64_t object_size;
};

static const struct vm_map vm_map_versions[] = {
    
    {
        .hdr_links_prev             = 0x10,
        .hdr_links_next             = 0x18,
        .min_offset                 = 0x20,
        .max_offset                 = 0x28,
        .hdr_nentries               = 0x30,
        .hdr_nentries_u64           = 0x30,
        .hdr_rb_head_store_rbh_root = 0x38,
        .pmap                       = 0x40,
        .hint                       = 0x98,
        .hole_hint                  = 0xa0,
        .holes_list                 = 0xa8,
        .object_size                = 0xc0,
    }, // iOS 16.0 - 16.1 arm64
    
    {
        .hdr_links_prev             = 0x10,
        .hdr_links_next             = 0x18,
        .min_offset                 = 0x20,
        .max_offset                 = 0x28,
        .hdr_nentries               = 0x30,
        .hdr_nentries_u64           = 0x30,
        .hdr_rb_head_store_rbh_root = 0x38,
        .pmap                       = 0x40,
        .hint                       = 0x98,
        .hole_hint                  = 0xa0,
        .holes_list                 = 0xa8,
        .object_size                = 0xc0,
    }, // iOS 16.2 - 16.3 arm64
    
    {
        .hdr_links_prev             = 0x10,
        .hdr_links_next             = 0x18,
        .min_offset                 = 0x20,
        .max_offset                 = 0x28,
        .hdr_nentries               = 0x30,
        .hdr_nentries_u64           = 0x30,
        .hdr_rb_head_store_rbh_root = 0x38,
        .pmap                       = 0x40,
        .hint                       = 0x98,
        .hole_hint                  = 0xa0,
        .holes_list                 = 0xa8,
        .object_size                = 0xc0,
    }, // iOS 16.4 - 16.6 arm64
};

typedef uint64_t vm_map_hdr_links_prev_t;
typedef uint64_t vm_map_hdr_links_next_t;
typedef uint64_t vm_map_min_offset_t;
typedef uint64_t vm_map_max_offset_t;
typedef int32_t vm_map_hdr_nentries_t;
typedef uint64_t vm_map_hdr_nentries_u64_t;
typedef uint64_t vm_map_hdr_rb_head_store_rbh_root_t;
typedef uint64_t vm_map_pmap_t;
typedef uint64_t vm_map_hint_t;
typedef uint64_t vm_map_hole_hint_t;
typedef uint64_t vm_map_holes_list_t;

struct _vm_map {
    uint64_t lock[2];
    struct vm_map_header hdr;
    uint64_t pmap;
    uint64_t size;
    uint64_t size_limit;
    uint64_t data_limit;
    uint64_t user_wire_limit;
    uint64_t user_wire_size;
#if TARGET_MACOS
    uint64_t vmmap_high_start;
#else /* TARGET_MACOS */
    uint64_t user_range[4];
#endif /* TARGET_MACOS */
    union {
        uint64_t vmu1_highest_entry_end;
        uint64_t vmu1_lowest_unnestable_start;
    } vmu1;
    uint64_t hint;
    union {
        uint64_t vmmap_hole_hint;
        uint64_t vmmap_corpse_footprint;
    } vmmap_u_1;
    union {
        uint64_t _first_free;
        uint64_t _holes;
    } f_s;
    uint32_t map_refcnt;
    uint32_t
        wait_for_space:1,
        wiring_required:1,
        no_zero_fill:1,
        mapped_in_other_pmaps:1,
        switch_protect:1,
        disable_vmentry_reuse:1,
        map_disallow_data_exec:1,
        holelistenabled:1,
        is_nested_map:1,
        map_disallow_new_exec:1,
        jit_entry_exists:1,
        has_corpse_footprint:1,
        terminated:1,
        is_alien:1,
        cs_enforcement:1,
        cs_debugged:1,
        reserved_regions:1,
        single_jit:1,
        never_faults:1,
        uses_user_ranges:1,
        pad:12;
    uint32_t timestamp;
};

#endif /* vm_map_h */
