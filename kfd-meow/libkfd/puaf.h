/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef puaf_h
#define puaf_h

#include "../libkfd.h"

#include "puaf/smith.h"
#include "puaf/physpuppet.h"

// Forward declarations for helper functions.
extern void puaf_helper_get_vm_map_first_and_last(uint64_t* first_out, uint64_t* last_out);
extern void puaf_helper_get_vm_map_min_and_max(uint64_t* min_out, uint64_t* max_out);
extern void puaf_helper_give_ppl_pages(void);

extern void puaf_init(struct kfd* kfd, uint64_t exploit_type);
extern void puaf_run(struct kfd* kfd);
extern void puaf_cleanup(struct kfd* kfd);
extern void puaf_free(struct kfd* kfd);


#endif /* puaf_h */
