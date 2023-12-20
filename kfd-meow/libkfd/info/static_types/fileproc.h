/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef fileproc_h
#define fileproc_h

struct fileproc {
    uint32_t fp_iocount;
    uint32_t fp_vflags;
    uint16_t fp_flags;
    uint16_t fp_guard_attrs;
    uint64_t fp_glob;
    union {
        uint64_t fp_wset;
        uint64_t fp_guard;
    };
};

#endif /* fileproc_h */
