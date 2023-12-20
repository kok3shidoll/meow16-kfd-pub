/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef pseminfo_h
#define pseminfo_h

struct pseminfo {
    uint32_t psem_flags;
    uint32_t psem_usecount;
    uint16_t psem_mode;
    uint32_t psem_uid;
    uint32_t psem_gid;
    char psem_name[32];
    uint64_t psem_semobject;
    uint64_t psem_label;
    int32_t psem_creator_pid;
    uint64_t psem_creator_uniqueid;
};

#endif /* pseminfo_h */
