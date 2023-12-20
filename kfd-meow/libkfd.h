/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef libkfd_h
#define libkfd_h

#include <stdint.h>
/*
 * The global configuration parameters of libkfd.
 */
#define CONFIG_ASSERT 1
#define CONFIG_PRINT  1
#define CONFIG_TIMER  1

#define MEOW_EXPLOIT_PHYSPUPPET (1 << 0)
#define MEOW_EXPLOIT_SMITH      (1 << 1)

#define KERNEL_BASE_ADDRESS     0xFFFFFFF007004000

#include "libkfd/common.h"

/*
 * The private API of libkfd.
 */

struct kfd; // Forward declaration for function pointers.

struct info {
    struct {
        vm_address_t src_uaddr;
        vm_address_t dst_uaddr;
        vm_size_t    size;
    } copy;
    struct {
        int32_t  pid;
        uint64_t tid;
        uint64_t vid;
        bool     ios;
        uint64_t maxfilesperproc;
    } env;
    struct {
        uint64_t kernel_slide;
        uint64_t gVirtBase;
        uint64_t gPhysBase;
        uint64_t gPhysSize;
        struct {
            uint64_t pa;
            uint64_t va;
        } ttbr[2];
        struct ptov_table_entry {
            uint64_t pa;
            uint64_t va;
            uint64_t len;
        } ptov_table[8];

        uint64_t current_map;
        uint64_t current_pmap;
        uint64_t current_proc;
        uint64_t current_task;
        uint64_t current_thread;
        uint64_t current_uthread;
        uint64_t kernel_map;
        uint64_t kernel_pmap;
        uint64_t kernel_proc;
        uint64_t kernel_task;
    } kernel;
};

struct perf {
    uint64_t kernelcache_index;
    struct {
        uint64_t kaddr;
        uint64_t paddr;
        uint64_t uaddr;
        uint64_t size;
    } shared_page;
    struct {
        int32_t  fd;
        uint32_t si_rdev_buffer[2];
        uint64_t si_rdev_kaddr;
    } dev;
    void (*saved_kread)(struct kfd*, uint64_t, void*, uint64_t);
    void (*saved_kwrite)(struct kfd*, void*, uint64_t, uint64_t);
};

struct puaf {
    uint64_t  number_of_puaf_pages;
    uint64_t* puaf_pages_uaddr;
    void*     puaf_method_data;
    uint64_t  puaf_method_data_size;
    struct {
        void (*init)(struct kfd*);
        void (*run)(struct kfd*);
        void (*cleanup)(struct kfd*);
        void (*free)(struct kfd*);
    } puaf_method_ops;
};

struct krkw {
    uint64_t krkw_maximum_id;
    uint64_t krkw_allocated_id;
    uint64_t krkw_searched_id;
    uint64_t krkw_object_id;
    uint64_t krkw_object_uaddr;
    uint64_t krkw_object_size;
    void*    krkw_method_data;
    uint64_t krkw_method_data_size;
    struct {
        void (*init)(struct kfd*);
        void (*allocate)(struct kfd*, uint64_t);
        bool (*search)(struct kfd*, uint64_t);
        void (*kread)(struct kfd*, uint64_t, void*, uint64_t);
        void (*kwrite)(struct kfd*, void*, uint64_t, uint64_t);
        void (*find_proc)(struct kfd*);
        void (*deallocate)(struct kfd*, uint64_t);
        void (*free)(struct kfd*);
    } krkw_method_ops;
};

struct kfd {
    struct info info;
    struct perf perf;
    struct puaf puaf;
    struct krkw kread;
    struct krkw kwrite;
};

extern uint64_t _kfd;

/*
 * The public API of libkfd.
 */

int isAvailable(void);

struct kfd* kfd_init(uint64_t exploit_type);
void kfd_free(struct kfd* kfd);
uint64_t kopen(uint64_t exploit_type);
void kread_kfd(uint64_t kfd, uint64_t va, void* ua, uint64_t size);
void kwrite_kfd(uint64_t kfd, const void* ua, uint64_t va, uint64_t size);
void kclose(uint64_t kfd);
void kreadbuf_kfd(uint64_t va, void* ua, size_t size);
void kwritebuf_kfd(uint64_t va, const void* ua, size_t size);
uint64_t kread64_kfd(uint64_t va);
void kwrite64_kfd(uint64_t va, uint64_t val);
uint64_t get_kaslr_slide(void);
uint64_t get_kernel_proc(void);
uint64_t get_kernel_task(void);
uint64_t get_current_proc(void);
uint64_t get_current_task(void);
uint64_t get_kernel_pmap(void);
uint64_t get_current_pmap(void);

uint64_t get_kernel_slide(void);

#endif /* libkfd_h */
