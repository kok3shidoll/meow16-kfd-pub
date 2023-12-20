/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef common_h
#define common_h

#include <errno.h>
#include <mach/mach.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/sysctl.h>
#include <unistd.h>

#define pages(number_of_pages) ((number_of_pages) * (16384ull))

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

/*
 * Helper print macros.
 */

#if CONFIG_PRINT

#define print(args...) printf(args)

#else /* CONFIG_PRINT */

#define print(args...)

#endif /* CONFIG_PRINT */

#define print_bool(name) print("[%s]: %s = %s\n", __FUNCTION__, #name, name ? "true" : "false")

#define print_i8(name) print("[%s]: %s = %hhi\n", __FUNCTION__, #name, name)
#define print_u8(name) print("[%s]: %s = %hhu\n", __FUNCTION__, #name, name)
#define print_x8(name) print("[%s]: %s = %02hhx\n", __FUNCTION__, #name, name)

#define print_i16(name) print("[%s]: %s = %hi\n", __FUNCTION__, #name, name)
#define print_u16(name) print("[%s]: %s = %hu\n", __FUNCTION__, #name, name)
#define print_x16(name) print("[%s]: %s = %04hx\n", __FUNCTION__, #name, name)

#define print_i32(name) print("[%s]: %s = %i\n", __FUNCTION__, #name, name)
#define print_u32(name) print("[%s]: %s = %u\n", __FUNCTION__, #name, name)
#define print_x32(name) print("[%s]: %s = %08x\n", __FUNCTION__, #name, name)

#define print_i64(name) print("[%s]: %s = %lli\n", __FUNCTION__, #name, name)
#define print_u64(name) print("[%s]: %s = %llu\n", __FUNCTION__, #name, name)
#define print_x64(name) print("[%s]: %s = %016llx\n", __FUNCTION__, #name, name)

#define print_isize(name) print("[%s]: %s = %li\n", __FUNCTION__, #name, name)
#define print_usize(name) print("[%s]: %s = %lu\n", __FUNCTION__, #name, name)
#define print_xsize(name) print("[%s]: %s = %016lx\n", __FUNCTION__, #name, name)

#define print_string(name) print("[%s]: %s = %s\n", __FUNCTION__, #name, name)

#define print_message(args...) do { print("[%s:msg]: ", __FUNCTION__); print(args); print("\n"); } while (0)
#define print_success(args...) do { print("[%s:log]: ", __FUNCTION__); print(args); print("\n"); } while (0)
#define print_warning(args...) do { print("[%s:warn]: ", __FUNCTION__); print(args); print("\n"); } while (0)
#define print_failure(args...) do { print("[%s:err]: ", __FUNCTION__); print(args); print("\n"); } while (0)

#define print_buffer(uaddr, size)                                            \
    do {                                                                     \
        const uint64_t u64_per_line = 8;                                     \
        volatile uint64_t* u64_base = (volatile uint64_t*)(uaddr);           \
        uint64_t u64_size = ((uint64_t)(size) / sizeof(uint64_t));           \
        for (uint64_t u64_offset = 0; u64_offset < u64_size; u64_offset++) { \
            if ((u64_offset % u64_per_line) == 0) {                          \
                print("[0x%04llx]: ", u64_offset * sizeof(uint64_t));        \
            }                                                                \
            print("%016llx", u64_base[u64_offset]);                          \
            if ((u64_offset % u64_per_line) == (u64_per_line - 1)) {         \
                print("\n");                                                 \
            } else {                                                         \
                print(" ");                                                  \
            }                                                                \
        }                                                                    \
        if ((u64_size % u64_per_line) != 0) {                                \
            print("\n");                                                     \
        }                                                                    \
    } while (0)

/*
 * Helper assert macros.
 */

#if CONFIG_ASSERT

#define kfd_assert(condition)                                           \
    do {                                                                \
        if (!(condition)) {                                             \
            print_failure("assertion failed: (%s)", #condition);        \
            print_failure("file: %s, line: %d", __FILE__, __LINE__);    \
            print_failure("... sleep(30) before exit(1) ...");          \
            sleep(30);                                                  \
            exit(1);                                                    \
        }                                                               \
    } while (0)

#else /* CONFIG_ASSERT */

#define kfd_assert(condition)

#endif /* CONFIG_ASSERT */

#define assert_false(message)                   \
    do {                                        \
        print_failure("error: %s", message);    \
        kfd_assert(false);                      \
    } while (0)

#define assert_bsd(statement)                                                                        \
    do {                                                                                             \
        kern_return_t kret = (statement);                                                            \
        if (kret != KERN_SUCCESS) {                                                                  \
            print_failure("bsd error: kret = %d, errno = %d (%s)", kret, errno, strerror(errno));    \
            kfd_assert(kret == KERN_SUCCESS);                                                        \
        }                                                                                            \
    } while (0)

#define assert_mach(statement)                                                             \
    do {                                                                                   \
        kern_return_t kret = (statement);                                                  \
        if (kret != KERN_SUCCESS) {                                                        \
            print_failure("mach error: kret = %d (%s)", kret, mach_error_string(kret));    \
            kfd_assert(kret == KERN_SUCCESS);                                              \
        }                                                                                  \
    } while (0)

/*
 * Helper allocation macros.
 */

#define malloc_bzero(size)               \
    ({                                   \
        void* pointer = malloc(size);    \
        kfd_assert(pointer != NULL);     \
        bzero(pointer, size);            \
        pointer;                         \
    })

#define bzero_free(pointer, size)    \
    do {                             \
        bzero(pointer, size);        \
        free(pointer);               \
        pointer = NULL;              \
    } while (0)

#endif /* common_h */
