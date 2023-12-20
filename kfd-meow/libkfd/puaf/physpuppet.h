/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef physpuppet_h
#define physpuppet_h

#include "../../libkfd.h"

#include "../puaf.h"
#include "../info.h"
#include "../info/dynamic_types/vm_map.h"
#include "../info/static_types/vm_map_entry.h"

void physpuppet_init(struct kfd* kfd);
void physpuppet_run(struct kfd* kfd);
void physpuppet_cleanup(struct kfd* kfd);
void physpuppet_free(struct kfd* kfd);

#endif /* physpuppet_h */
