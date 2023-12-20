//
//  libmeow.m
//  kfd-meow
//
//  Created by doraaa on 2023/12/17.
//

#include "libmeow.h"

uint64_t kernel_base = 0;
uint64_t kernel_slide = 0;

uint64_t our_task = 0;
uint64_t our_proc = 0;
uint64_t kernel_task = 0;
uint64_t kernproc = 0;
uint64_t our_ucred = 0;
uint64_t kern_ucred = 0;

uint64_t gCpuTTEP = 0;
uint64_t gPhysBase = 0;
uint64_t gVirtBase = 0;

uint64_t data__gCpuTTEP = 0;
uint64_t data__gVirtBase = 0;
uint64_t data__gPhysBase = 0;

uint64_t func__proc_set_ucred = 0;

void set_offsets(void) {
    kernel_slide = get_kernel_slide();
    kernel_base = kernel_slide + KERNEL_BASE_ADDRESS;
    our_task = get_current_task();
    our_proc = get_current_proc();
    kernel_task = get_kernel_task();
    kernproc = get_kernel_proc();
    our_ucred = proc_get_ucred(our_proc);
    kern_ucred = proc_get_ucred(kernproc);
    
    printf("kernel_slide : %016llx\n", kernel_slide);
    printf("kernel_base  : %016llx\n", kernel_base);
    printf("our_task     : %016llx\n", our_task);
    printf("our_proc     : %016llx\n", our_proc);
    printf("kernel_task  : %016llx\n", kernel_task);
    printf("kernproc     : %016llx\n", kernproc);
    printf("our_ucred    : %016llx\n", our_ucred);
    printf("kern_ucred   : %016llx\n", kern_ucred);
}

/*---- kvrw ----*/
uint8_t kread8(uint64_t va) {
    union {
        uint8_t u8[8];
        uint64_t u64;
    } u;
    u.u64 = kread64_kfd(va);
    return u.u8[0];
}

uint16_t kread16(uint64_t va) {
    union {
        uint16_t u16[4];
        uint64_t u64;
    } u;
    u.u64 = kread64_kfd(va);
    return u.u16[0];
}

uint32_t kread32(uint64_t va) {
    union {
        uint32_t u32[2];
        uint64_t u64;
    } u;
    u.u64 = kread64_kfd(va);
    return u.u32[0];
}

uint64_t kread64(uint64_t va) {
    return kread64_kfd(va);
}

void kreadbuf(uint64_t va, void* ua, size_t size) {
    kreadbuf_kfd(va, ua, size);
}

void kwrite64(uint64_t va, uint64_t val) {
    kwrite64_kfd(va, val);
}

void kwrite32(uint64_t va, uint32_t val) {
    union {
        uint32_t u32[2];
        uint64_t u64;
    } u;
    u.u64 = kread64_kfd(va);
    u.u32[0] = val;
    kwrite64(va, u.u64);
}

void kwrite16(uint64_t va, uint16_t val) {
    union {
        uint16_t u16[4];
        uint64_t u64;
    } u;
    u.u64 = kread64_kfd(va);
    u.u16[0] = val;
    kwrite64(va, u.u64);
}

void kwrite8(uint64_t va, uint8_t val) {
    union {
        uint8_t u8[8];
        uint64_t u64;
    } u;
    u.u64 = kread64_kfd(va);
    u.u8[0] = val;
    kwrite64(va, u.u64);
}

void kwritebuf(uint64_t va, const void* ua, size_t size) {
    kwritebuf_kfd(va, ua, size);
}

/*---- proc ----*/
uint64_t proc_get_proc_ro(uint64_t proc_ptr) {
    return kread64(proc_ptr + 0x18);
}

uint64_t proc_ro_get_ucred(uint64_t proc_ro_ptr) {
    return kread64(proc_ro_ptr + 0x20);
}

uint64_t proc_get_ucred(uint64_t proc_ptr) {
    return proc_ro_get_ucred(proc_get_proc_ro(proc_ptr));
}

/*---- meow ----*/
int meow(void) {
    
    set_offsets();
    
    offsetfinder64_kread();
    
    return 0;
}
