/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef fileglob_h
#define fileglob_h

struct fileglob {
    struct {
        uint64_t le_next;
        uint64_t le_prev;
    } _msglist;
    uint32_t fg_flag;
    uint32_t fg_count;
    uint32_t fg_msgcount;
    int32_t fg_lflags;
    uint64_t fg_cred;
    uint64_t fg_ops;
    int64_t fg_offset;
    uint64_t fg_data;
    uint64_t fg_vn_data;
    uint64_t fg_lock[2];
};

#endif /* fileglob_h */
