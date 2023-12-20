/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef pmap_h
#define pmap_h

struct pmap {
    uint64_t tte;
    uint64_t ttep;
    uint64_t min;
    uint64_t max;
    uint64_t pmap_pt_attr;
    uint64_t ledger;
    uint64_t rwlock[2];
    struct {
        uint64_t next;
        uint64_t prev;
    } pmaps;
    uint64_t tt_entry_free;
    uint64_t nested_pmap;
    uint64_t nested_region_addr;
    uint64_t nested_region_size;
    uint64_t nested_region_true_start;
    uint64_t nested_region_true_end;
    uint64_t nested_region_asid_bitmap;
    uint32_t nested_region_asid_bitmap_size;
    uint64_t reserved0;
    uint64_t reserved1;
    uint64_t reserved2;
    uint64_t reserved3;
    int32_t ref_count;
    int32_t nested_count;
    uint32_t nested_no_bounds_refcnt;
    uint16_t hw_asid;
    uint8_t sw_asid;
    bool reserved4;
    bool pmap_vm_map_cs_enforced;
    bool reserved5;
    uint32_t reserved6;
    uint8_t reserved7;
    uint8_t type;
    bool reserved8;
    bool reserved9;
    bool is_rosetta;
    bool nx_enabled;
    bool is_64bit;
    bool nested_has_no_bounds_ref;
    bool nested_bounds_set;
    bool disable_jop;
    bool reserved11;
};

#endif /* pmap_h */
