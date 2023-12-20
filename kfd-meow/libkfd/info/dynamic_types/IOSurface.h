//
//  IOSurface.h
//  kfd
//
//  Created by Lars Fröder on 30.07.23.
//

#ifndef IOSurface_h
#define IOSurface_h

struct IOSurface {
    uint64_t isa;
    uint64_t PixelFormat;
    uint64_t AllocSize;
    uint64_t UseCountPtr;
    uint64_t IndexedTimestampPtr;
    
    uint64_t ReadDisplacement;
};

static const struct IOSurface IOSurface_versions[] = {
    
    {
        .isa                    = 0x0,
        .PixelFormat            = 0xA4,
        .AllocSize              = 0xAC,
        .UseCountPtr            = 0xC0,
        .IndexedTimestampPtr    = 0x368,
        .ReadDisplacement       = 0x18
    }, // iOS 16.0 - 16.1 arm64
    
    {
        .isa                    = 0x0,
        .PixelFormat            = 0xA4,
        .AllocSize              = 0xAC,
        .UseCountPtr            = 0xC0,
        .IndexedTimestampPtr    = 0x368,
        .ReadDisplacement       = 0x18
    }, // iOS 16.2 - 16.3 arm64
    
    {
        .isa                    = 0x0,
        .PixelFormat            = 0xA4,
        .AllocSize              = 0xAC,
        .UseCountPtr            = 0xC0,
        .IndexedTimestampPtr    = 0x368,
        .ReadDisplacement       = 0x18
    }, // iOS 16.4 - 16.6 arm64
};

typedef uint64_t IOSurface_isa_t;
typedef uint32_t IOSurface_PixelFormat_t;
typedef uint32_t IOSurface_AllocSize_t;
typedef uint64_t IOSurface_UseCountPtr_t;
typedef uint64_t IOSurface_IndexedTimestampPtr_t;
typedef uint32_t IOSurface_ReadDisplacement_t;


#endif /* IOSurface_h */
