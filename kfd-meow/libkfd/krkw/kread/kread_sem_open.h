/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef kread_sem_open_h
#define kread_sem_open_h

#include <assert.h>
#include "../../../libkfd.h"

#include "../../info.h"
#include "../../info/static_types/semaphore.h"
#include "../../info/static_types/miscellaneous_types.h"
#include "../../info/static_types/pseminfo.h"
#include "../../info/static_types/psemnode.h"
#include "../../info/dynamic_types/proc.h"

uint64_t kread_sem_open_kread_u64(struct kfd* kfd, uint64_t kaddr);
uint32_t kread_sem_open_kread_u32(struct kfd* kfd, uint64_t kaddr);

void kread_sem_open_init(struct kfd* kfd);
void kread_sem_open_allocate(struct kfd* kfd, uint64_t id);
bool kread_sem_open_search(struct kfd* kfd, uint64_t object_uaddr);
void kread_sem_open_kread(struct kfd* kfd, uint64_t kaddr, void* uaddr, uint64_t size);
void kread_sem_open_find_proc(struct kfd* kfd);
void kread_sem_open_deallocate(struct kfd* kfd, uint64_t id);

void kread_sem_open_free(struct kfd* kfd);

/*
 * 64-bit kread function.
 */
uint64_t kread_sem_open_kread_u64(struct kfd* kfd, uint64_t kaddr);

/*
 * 32-bit kread function that is guaranteed to not underflow a page,
 * i.e. those 4 bytes are the first 4 bytes read by the modified kernel pointer.
 */
uint32_t kread_sem_open_kread_u32(struct kfd* kfd, uint64_t kaddr);

#endif /* kread_sem_open_h */
