/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef ipc_entry_h
#define ipc_entry_h

struct ipc_entry {
    union {
        uint64_t ie_object;
        uint64_t ie_volatile_object;
    };
    uint32_t ie_bits;
    uint32_t ie_dist:12;
    uint32_t ie_index:32;
    union {
        uint32_t ie_next;
        uint32_t ie_request;
    };
};

#endif /* ipc_entry_h */
