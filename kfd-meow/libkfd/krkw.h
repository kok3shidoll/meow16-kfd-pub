/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef krkw_h
#define krkw_h

#include "../libkfd.h"

#include "info.h"
#include "krkw/kread/kread_sem_open.h"
#include "krkw/kwrite/kwrite_IOSurface.h"

// Forward declarations for helper functions.
void krkw_helper_init(struct kfd* kfd, struct krkw* krkw);
void krkw_helper_grab_free_pages(struct kfd* kfd);
void krkw_helper_run_allocate(struct kfd* kfd, struct krkw* krkw);
void krkw_helper_run_deallocate(struct kfd* kfd, struct krkw* krkw);
void krkw_helper_free(struct kfd* kfd, struct krkw* krkw);

void krkw_init(struct kfd* kfd);
void krkw_run(struct kfd* kfd);
void krkw_kread(struct kfd* kfd, uint64_t kaddr, void* uaddr, uint64_t size);
void krkw_kwrite(struct kfd* kfd, void* uaddr, uint64_t kaddr, uint64_t size);
void krkw_free(struct kfd* kfd);

#endif /* krkw_h */
