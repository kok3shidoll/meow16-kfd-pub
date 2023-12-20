/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef vm_named_entry_h
#define vm_named_entry_h

struct vm_named_entry {
    uint64_t Lock[2];
    union {
        uint64_t map;
        uint64_t copy;
    } backing;
    uint64_t offset;
    uint64_t size;
    uint64_t data_offset;
    uint32_t
        protection:4,
        is_object:1,
        internal:1,
        is_sub_map:1,
        is_copy:1,
        is_fully_owned:1;
};

#endif /* vm_named_entry_h */
