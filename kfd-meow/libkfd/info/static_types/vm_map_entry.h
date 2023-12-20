/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef vm_map_entry_h
#define vm_map_entry_h

#include "vm_object.h"

#define vme_prev     links.prev
#define vme_next     links.next
#define vme_start    links.start
#define vme_end      links.end

struct vm_map_links {
    uint64_t prev;
    uint64_t next;
    uint64_t start;
    uint64_t end;
};

struct vm_map_store {
    struct {
        uint64_t rbe_left;
        uint64_t rbe_right;
        uint64_t rbe_parent;
    } entry;
};

struct vm_map_entry {
    struct vm_map_links links;
    struct vm_map_store store;
    union {
        uint64_t vme_object_value;
        struct {
            uint64_t vme_atomic:1;
            uint64_t is_sub_map:1;
            uint64_t vme_submap:60;
        };
        struct {
            uint32_t vme_ctx_atomic:1;
            uint32_t vme_ctx_is_sub_map:1;
            uint32_t vme_context:30;
            uint32_t vme_object;
        };
    };
    uint64_t
        vme_alias:12,
        vme_offset:52,
        is_shared:1,
        __unused1:1,
        in_transition:1,
        needs_wakeup:1,
        behavior:2,
        needs_copy:1,
        protection:3,
        used_for_tpro:1,
        max_protection:4,
        inheritance:2,
        use_pmap:1,
        no_cache:1,
        vme_permanent:1,
        superpage_size:1,
        map_aligned:1,
        zero_wired_pages:1,
        used_for_jit:1,
        pmap_cs_associated:1,
        iokit_acct:1,
        vme_resilient_codesign:1,
        vme_resilient_media:1,
        __unused2:1,
        vme_no_copy_on_read:1,
        translated_allow_execute:1,
        vme_kernel_object:1;
    uint16_t wired_count;
    uint16_t user_wired_count;
};

#define vme_for_store(kaddr) ((kaddr) ? (((kaddr) - sizeof(struct vm_map_links)) & (~1ull)) : (kaddr))
#define store_for_vme(kaddr) ((kaddr) ? (((kaddr) + sizeof(struct vm_map_links))) : (kaddr))

#endif /* vm_map_entry_h */
