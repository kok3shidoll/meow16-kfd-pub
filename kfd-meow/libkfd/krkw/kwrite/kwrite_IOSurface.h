//
//  kwrite_IOSurface.h
//  kfd
//
//  Created by Lars Fröder on 30.07.23.
//
// I attempted to make this standalone from kread but that probably doesn't work, so just select IOSurface for both kread and kwrite

#ifndef kwrite_IOSurface_h
#define kwrite_IOSurface_h

#include "../../../libkfd.h"

#include "../IOSurface_shared.h"
#include "../../info.h"
#include "../../info/dynamic_types/IOSurface.h"

#define IOSURFACE_MAGIC 0x1EA5CACE
extern io_connect_t g_surfaceConnect;

void kwrite_IOSurface_init(struct kfd* kfd);
void kwrite_IOSurface_allocate(struct kfd* kfd, uint64_t id);
bool kwrite_IOSurface_search(struct kfd* kfd, uint64_t object_uaddr);
void kwrite_IOSurface_kwrite(struct kfd* kfd, void* uaddr, uint64_t kaddr, uint64_t size);
void kwrite_IOSurface_find_proc(struct kfd* kfd);
void kwrite_IOSurface_deallocate(struct kfd* kfd, uint64_t id);
void kwrite_IOSurface_free(struct kfd* kfd);
void kwrite_IOSurface_kwrite_u64(struct kfd* kfd, uint64_t kaddr, uint64_t new_value);

#endif /* kwrite_IOSurface_h */
