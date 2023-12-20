/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef vm_object_h
#define vm_object_h

#include <stdint.h>
#include "vm_page.h"
#include "../../../libkfd.h"

#define vo_size                   vo_un1.vou_size
#define vo_cache_pages_to_scan    vo_un1.vou_cache_pages_to_scan
#define vo_shadow_offset          vo_un2.vou_shadow_offset
#define vo_cache_ts               vo_un2.vou_cache_ts
#define vo_owner                  vo_un2.vou_owner

struct vm_object {
    vm_page_queue_head_t memq;
    uint64_t Lock[2];
    union {
        uint64_t vou_size;
        int32_t vou_cache_pages_to_scan;
    } vo_un1;
    uint64_t memq_hint;
    int32_t ref_count;
    uint32_t resident_page_count;
    uint32_t wired_page_count;
    uint32_t reusable_page_count;
    uint64_t copy;
    uint64_t shadow;
    uint64_t pager;
    union {
        uint64_t vou_shadow_offset;
        uint64_t vou_cache_ts;
        uint64_t vou_owner;
    } vo_un2;
    uint64_t paging_offset;
    uint64_t pager_control;
    int32_t copy_strategy;
    uint32_t
        paging_in_progress:16,
        __object1_unused_bits:16;
    uint32_t activity_in_progress;
    uint32_t
             all_wanted:11,
             pager_created:1,
             pager_initialized:1,
             pager_ready:1,
             pager_trusted:1,
             can_persist:1,
             internal:1,
             private:1,
             pageout:1,
             alive:1,
             purgable:2,
             purgeable_only_by_kernel:1,
             purgeable_when_ripe:1,
             shadowed:1,
             true_share:1,
             terminating:1,
             named:1,
             shadow_severed:1,
             phys_contiguous:1,
             nophyscache:1,
             for_realtime:1;
    queue_chain_t cached_list;
    uint64_t last_alloc;
    uint64_t cow_hint;
    int32_t sequential;
    uint32_t pages_created;
    uint32_t pages_used;
    uint32_t
             wimg_bits:8,
             code_signed:1,
             transposed:1,
             mapping_in_progress:1,
             phantom_isssd:1,
             volatile_empty:1,
             volatile_fault:1,
             all_reusable:1,
             blocked_access:1,
             set_cache_attr:1,
             object_is_shared_cache:1,
             purgeable_queue_type:2,
             purgeable_queue_group:3,
             io_tracking:1,
             no_tag_update:1,
             eligible_for_secluded:1,
             can_grab_secluded:1,
             __unused_access_tracking:1,
             vo_ledger_tag:3,
             vo_no_footprint:1;
    uint8_t scan_collisions;
    uint8_t __object4_unused_bits[1];
    uint16_t wire_tag;
    uint32_t phantom_object_id;
    queue_head_t uplq;
    queue_chain_t objq;
    queue_chain_t task_objq;
};


#endif /* vm_object_h */
