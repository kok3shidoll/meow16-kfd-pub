/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef vm_page_h
#define vm_page_h

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint64_t next;
    uint64_t prev;
} queue_head_t, queue_chain_t;

typedef struct {
    uint32_t next;
    uint32_t prev;
} vm_page_queue_head_t, vm_page_queue_chain_t;

#define vmp_pageq    vmp_q_un.vmp_q_pageq
#define vmp_snext    vmp_q_un.vmp_q_snext

struct vm_page {
    union {
        vm_page_queue_chain_t vmp_q_pageq;
        uint64_t vmp_q_snext;
    } vmp_q_un;
    vm_page_queue_chain_t vmp_listq;
    vm_page_queue_chain_t vmp_specialq;
    uint64_t vmp_offset;
    uint32_t vmp_object;
    uint32_t
             vmp_wire_count:16,
             vmp_q_state:4,
             vmp_on_specialq:2,
             vmp_gobbled:1,
             vmp_laundry:1,
             vmp_no_cache:1,
             vmp_private:1,
             vmp_reference:1,
             vmp_lopage:1,
             vmp_realtime:1,
             vmp_unused_page_bits:3;
    uint32_t vmp_next_m;
    uint32_t
             vmp_busy:1,
             vmp_wanted:1,
             vmp_tabled:1,
             vmp_hashed:1,
             vmp_fictitious:1,
             vmp_clustered:1,
             vmp_pmapped:1,
             vmp_xpmapped:1,
             vmp_wpmapped:1,
             vmp_free_when_done:1,
             vmp_absent:1,
             vmp_error:1,
             vmp_dirty:1,
             vmp_cleaning:1,
             vmp_precious:1,
             vmp_overwriting:1,
             vmp_restart:1,
             vmp_unusual:1,
             vmp_cs_validated:4,
             vmp_cs_tainted:4,
             vmp_cs_nx:4,
             vmp_reusable:1,
             vmp_written_by_kernel:1;
};

extern struct vm_page* vm_pages;
extern struct vm_page* vm_page_array_beginning_addr;
extern struct vm_page* vm_page_array_ending_addr;
extern uint32_t vm_first_phys_ppnum;

#define __WORDSIZE 64

#define TiB(x) ((0ull + (x)) << 40)
#define GiB(x) ((0ull + (x)) << 30)

#if TARGET_MACOS
#define VM_KERNEL_POINTER_SIGNIFICANT_BITS 41
#define VM_MIN_KERNEL_ADDRESS ((uint64_t)(0ull - TiB(2)))
#else /* TARGET_MACOS */
#define VM_KERNEL_POINTER_SIGNIFICANT_BITS 38
#define VM_MIN_KERNEL_ADDRESS ((uint64_t)(0ull - GiB(144)))
#endif /* TARGET_MACOS */

#define VM_MIN_KERNEL_AND_KEXT_ADDRESS VM_MIN_KERNEL_ADDRESS

#define VM_PAGE_PACKED_PTR_ALIGNMENT    64
#define VM_PAGE_PACKED_ALIGNED          __attribute__((aligned(VM_PAGE_PACKED_PTR_ALIGNMENT)))
#define VM_PAGE_PACKED_PTR_BITS         31
#define VM_PAGE_PACKED_PTR_SHIFT        6
#define VM_PAGE_PACKED_PTR_BASE         ((uintptr_t)(VM_MIN_KERNEL_AND_KEXT_ADDRESS))
#define VM_PAGE_PACKED_FROM_ARRAY       0x80000000

typedef struct vm_packing_params {
    uint64_t vmpp_base;
    uint8_t vmpp_bits;
    uint8_t vmpp_shift;
    bool vmpp_base_relative;
} vm_packing_params_t;

#define VM_PACKING_IS_BASE_RELATIVE(ns) \
    (ns##_BITS + ns##_SHIFT <= VM_KERNEL_POINTER_SIGNIFICANT_BITS)

#define VM_PACKING_PARAMS(ns)                                     \
    (vm_packing_params_t) {                                       \
        .vmpp_base = ns##_BASE,                                   \
        .vmpp_bits = ns##_BITS,                                   \
        .vmpp_shift = ns##_SHIFT,                                 \
        .vmpp_base_relative = VM_PACKING_IS_BASE_RELATIVE(ns),    \
    }

uint64_t vm_unpack_pointer(uint64_t packed, vm_packing_params_t params);

#define VM_UNPACK_POINTER(packed, ns) \
    vm_unpack_pointer(packed, VM_PACKING_PARAMS(ns))

uint64_t vm_page_unpack_ptr(uint64_t packed_page);

#define VM_PAGE_UNPACK_PTR(p)    (vm_page_unpack_ptr((uint64_t)(p)))
#define VM_OBJECT_UNPACK(p)      ((uint64_t)(VM_UNPACK_POINTER(p, VM_PAGE_PACKED_PTR)))
#define VM_PAGE_OBJECT(p)        (VM_OBJECT_UNPACK((p)->vmp_object))

uint32_t VM_PAGE_GET_PHYS_PAGE(struct vm_page* p);

#endif /* vm_page_h */
