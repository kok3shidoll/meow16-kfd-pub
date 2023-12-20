/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef info_h
#define info_h

#include "../libkfd.h"

#include "info/dynamic_types/proc.h"
#include "info/dynamic_types/task.h"
#include "info/dynamic_types/thread.h"
#include "info/dynamic_types/vm_map.h"

/*
 * Helper macros for dynamic types.
 */

#define dynamic_sizeof(object) (object##_versions[kfd->info.env.vid].object_size)

#define dynamic_offsetof(object, field) (object##_versions[kfd->info.env.vid].field)

#define dynamic_uget(object, field, object_uaddr)                                             \
    ({                                                                                        \
        uint64_t field_uaddr = (uint64_t)(object_uaddr) + dynamic_offsetof(object, field);    \
        object##_##field##_t field_value = *(volatile object##_##field##_t*)(field_uaddr);    \
        field_value;                                                                          \
    })

#define dynamic_uset(object, field, object_uaddr, field_value)                                   \
    do {                                                                                         \
        uint64_t field_uaddr = (uint64_t)(object_uaddr) + dynamic_offsetof(object, field);       \
        *(volatile object##_##field##_t*)(field_uaddr) = (object##_##field##_t)(field_value);    \
    } while (0)

#define dynamic_kget(object, field, object_kaddr)                                          \
    ({                                                                                     \
        uint64_t buffer = 0;                                                               \
        uint64_t field_kaddr = (uint64_t)(object_kaddr) + dynamic_offsetof(object, field); \
        kread_kfd((uint64_t)(kfd), (field_kaddr), (&buffer), (sizeof(buffer)));            \
        object##_##field##_t field_value = *(object##_##field##_t*)(&buffer);              \
        field_value;                                                                       \
    })

#define dynamic_kset_u64(object, field, object_kaddr, field_value)                         \
    do {                                                                                   \
        uint64_t buffer = field_value;                                                     \
        uint64_t field_kaddr = (uint64_t)(object_kaddr) + dynamic_offsetof(object, field); \
        kwrite_kfd((uint64_t)(kfd), (&buffer), (field_kaddr), (sizeof(buffer)));           \
    } while (0)

/*
 * Helper macros for static types.
 */

#define static_sizeof(object) (sizeof(struct object))

#define static_offsetof(object, field) (offsetof(struct object, field))

#define static_uget(object, field, object_uaddr) (((volatile struct object*)(object_uaddr))->field)

#define static_uset(object, field, object_uaddr, field_value)                  \
    do {                                                                       \
        (((volatile struct object*)(object_uaddr))->field = (field_value));    \
    } while (0)

#define static_kget(object, field_type, field, object_kaddr)                              \
    ({                                                                                    \
        uint64_t buffer = 0;                                                              \
        uint64_t field_kaddr = (uint64_t)(object_kaddr) + static_offsetof(object, field); \
        kread_kfd((uint64_t)(kfd), (field_kaddr), (&buffer), (sizeof(buffer)));           \
        field_type field_value = *(field_type*)(&buffer);                                 \
        field_value;                                                                      \
    })

#define static_kset_u64(object, field, object_kaddr, field_value)                         \
    do {                                                                                  \
        uint64_t buffer = field_value;                                                    \
        uint64_t field_kaddr = (uint64_t)(object_kaddr) + static_offsetof(object, field); \
        kwrite_kfd((uint64_t)(kfd), (&buffer), (field_kaddr), (sizeof(buffer)));          \
    } while (0)

#define t1sz_boot (25ull)
#define ptr_mask ((1ull << (64ull - t1sz_boot)) - 1ull)
#define pac_mask (~ptr_mask)
#define unsign_kaddr(kaddr) ((kaddr) | (pac_mask))

extern const char copy_sentinel[16];
extern const uint64_t copy_sentinel_size;

void info_init(struct kfd* kfd);
void info_run(struct kfd* kfd);
void info_free(struct kfd* kfd);

#endif /* info_h */
