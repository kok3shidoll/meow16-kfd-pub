//
//  IOSurface_shared.h
//  kfd
//
//  Created by Lars Fröder on 29.07.23.
//  Mostly stolen from weightBufs: https://github.com/0x36/weightBufs

#ifndef IOSurface_shared_h
#define IOSurface_shared_h

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mach/mach.h>
#include <mach/mach_error.h>
#include <mach/mach_time.h>
#include <CoreFoundation/CoreFoundation.h>

#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#include "../../libkfd.h"

#define False  0
#define True   1


#define CHECK_IOKIT_ERR(kr,name)  do {                          \
                if (kr != KERN_SUCCESS) {                       \
                        printf("%s : %s (0x%x)\n",              \
                               name,mach_error_string(kr),kr);  \
                }                                               \
        }while(0);


CFNumberRef CFInt32(int32_t value);
CFNumberRef CFInt64(int64_t value);

enum {
        kOSSerializeDictionary   = 0x01000000U,
        kOSSerializeArray        = 0x02000000U,
        kOSSerializeSet          = 0x03000000U,
        kOSSerializeNumber       = 0x04000000U,
        kOSSerializeSymbol       = 0x08000000U,
        kOSSerializeString       = 0x09000000U,
        kOSSerializeData         = 0x0a000000U,
        kOSSerializeBoolean      = 0x0b000000U,
        kOSSerializeObject       = 0x0c000000U,
        kOSSerializeTypeMask     = 0x7F000000U,
        kOSSerializeDataMask     = 0x00FFFFFFU,

        kOSSerializeEndCollection = 0x80000000U,
};

// IOKIT
typedef mach_port_t io_connect_t;
typedef mach_port_t io_service_t;
typedef mach_port_t io_iterator_t;
typedef mach_port_t io_object_t;
typedef mach_port_t io_registry_entry_t;

#ifndef IO_OBJECT_NULL
# define IO_OBJECT_NULL 0
#endif

extern const mach_port_t kIOMasterPortDefault;

kern_return_t IOConnectCallMethod(mach_port_t connection, uint32_t selector, const uint64_t *input, uint32_t inputCnt, const void *inputStruct, size_t inputStructCnt, uint64_t *output, uint32_t *outputCnt, void *outputStruct, size_t *outputStructCnt);
kern_return_t IOConnectCallAsyncMethod(mach_port_t connection, uint32_t selector, mach_port_t wake_port, uint64_t *reference, uint32_t referenceCnt, const uint64_t *input, uint32_t inputCnt, const void *inputStruct, size_t inputStructCnt, uint64_t *output, uint32_t *outputCnt, void *outputStruct, size_t *outputStructCnt);
        kern_return_t IOConnectMapMemory(io_connect_t connect, uint32_t memoryType, task_port_t intoTask, mach_vm_address_t *atAddress, mach_vm_size_t *ofSize, uint32_t options);
io_service_t IOServiceGetMatchingService(mach_port_t masterPort, CFDictionaryRef matching);
kern_return_t IOServiceOpen(io_service_t service, task_port_t owningTask, uint32_t type,io_connect_t *connect);
kern_return_t IOServiceGetMatchingServices(mach_port_t masterPort, CFDictionaryRef matching, io_iterator_t *existing);
kern_return_t IOServiceClose(io_connect_t connect);
uint32_t IOObjectGetRetainCount(io_object_t object);
uint32_t IOObjectGetKernelRetainCount(io_object_t object);
uint32_t IOObjectGetRetainCount(io_object_t object);
kern_return_t io_object_get_retain_count(mach_port_t object, uint32_t *retainCount);
kern_return_t IOObjectRelease(io_object_t object);
kern_return_t IORegistryEntrySetCFProperties(io_registry_entry_t entry, CFTypeRef properties);
kern_return_t IOConnectSetNotificationPort(io_connect_t connect, uint32_t type, mach_port_t port, uintptr_t reference);
CFMutableDictionaryRef IOServiceMatching(const char *name);
CFDataRef IOCFSerialize(CFTypeRef object, CFOptionFlags options);
CFTypeRef IOCFUnserialize(const char *buffer, CFAllocatorRef allocator, CFOptionFlags options, CFStringRef *errorString);

typedef struct {
        io_connect_t client;
        uint8_t *sInput;
        uint32_t sInputSize;
        uint8_t * sOutput;
        size_t sOutputSize;
        mach_port_t asyncAwake;
        uint64_t references[8];
        uint32_t referenceCnt;
        uint64_t scalarI[0x10];
        uint64_t scalarO[0x10];
        uint32_t scalarISize;
        size_t scalarOSize;
} IOKit_args_t;

#ifdef __cplusplus
}
#endif

#include <IOSurface/IOSurfaceRef.h>

#define IOSurfaceLockResultSize 0xA68 // ios 16

#define kOSSerializeBinarySignature        0x000000D3
#define kOSSerializeIndexedBinarySignature 0x000000D4

typedef struct IOSurfaceFastCreateArgs
{
        uint64_t IOSurfaceAddress;
        uint32_t IOSurfaceWidth;
        uint32_t IOSurfaceHeight;
        uint32_t IOSurfacePixelFormat;
        uint32_t IOSurfaceBytesPerElement;
        uint32_t IOSurfaceBytesPerRow;
        uint32_t IOSurfaceAllocSize;
} IOSurfaceFastCreateArgs;

struct iosurface_obj {
    io_connect_t port;
    uint32_t surface_id;
};

io_connect_t iokit_get_connection(const char *name, unsigned int type);
io_connect_t get_surface_client(void);
io_connect_t create_surface_fast_path(io_connect_t surface, uint32_t *surface_id, IOSurfaceFastCreateArgs *args);
io_connect_t release_surface(io_connect_t surface, uint32_t surface_id);
void set_indexed_timestamp(io_connect_t c, uint32_t surface_id, uint64_t index, uint64_t value);
kern_return_t iosurface_get_use_count(io_connect_t c, uint32_t surface_id, uint32_t *output);

#endif /* IOSurface_shared_h */
