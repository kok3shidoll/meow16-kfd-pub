/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#include "perf.h"

void perf_init(struct kfd* kfd)
{
    char hw_model[16] = {};
    uintptr_t size = sizeof(hw_model);
    assert_bsd(sysctlbyname("hw.model", hw_model, &size, NULL, 0));
    print_string(hw_model);

    kfd->perf.kernelcache_index = 0;
    
    /*
     * Allocate a page that will be used as a shared buffer between user space and kernel space.
     */
    vm_address_t shared_page_address = 0;
    vm_size_t shared_page_size = pages(1);
    assert_mach(vm_allocate(mach_task_self(), &shared_page_address, shared_page_size, VM_FLAGS_ANYWHERE));
    memset((void*)(shared_page_address), 0, shared_page_size);
    kfd->perf.shared_page.uaddr = shared_page_address;
    kfd->perf.shared_page.size = shared_page_size;
}
