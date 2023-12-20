/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef ipc_space_h
#define ipc_space_h

struct ipc_space {
    uint64_t is_lock[2];
    uint32_t is_bits;
    uint32_t is_table_hashed;
    uint32_t is_table_free;
    uint64_t is_table;
    uint64_t is_task;
    uint64_t is_grower;
    uint64_t is_label;
    uint32_t is_low_mod;
    uint32_t is_high_mod;
    struct {
        uint32_t seed[4];
        uint32_t state;
        uint64_t lock[2];
    } bool_gen;
    uint32_t is_entropy[1];
    int32_t is_node_id;
};

#endif /* ipc_space_h */
