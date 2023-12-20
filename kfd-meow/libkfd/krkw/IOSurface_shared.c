//
//  IOSurface_shared.h
//  kfd
//
//  Created by Lars Fröder on 29.07.23.
//  Mostly stolen from weightBufs: https://github.com/0x36/weightBufs

#include "IOSurface_shared.h"

io_connect_t iokit_get_connection(const char *name, unsigned int type)
{
        io_service_t service  = IOServiceGetMatchingService(kIOMasterPortDefault,
                                                            IOServiceMatching(name));
        if (service == IO_OBJECT_NULL) {
                printf("unable to find service\n");
                exit(-1);
        }

        io_connect_t conn = MACH_PORT_NULL;
        kern_return_t kr = IOServiceOpen(service, mach_task_self(), type, &conn);
        if(kr != KERN_SUCCESS) {
                printf("[x] Could not open %s: %s\n", name, mach_error_string(kr));
                exit(-1);

        }
        return conn;
}


io_connect_t get_surface_client(void)
{
        return iokit_get_connection("IOSurfaceRoot", 0);
}


io_connect_t create_surface_fast_path(io_connect_t surface, uint32_t *surface_id, IOSurfaceFastCreateArgs *args)
{
        io_connect_t conn = surface;
        kern_return_t kr = KERN_SUCCESS;

        char output[IOSurfaceLockResultSize] = {0};
        size_t output_cnt = IOSurfaceLockResultSize;

        if (surface == 0) {
                conn = get_surface_client();
        }

        kr = IOConnectCallMethod(conn, 6, 0, 0,
                                 args, 0x20,
                                 NULL, NULL, output, &output_cnt);
        CHECK_IOKIT_ERR(kr, "create_surface_fast_path");
        assert(kr == KERN_SUCCESS);

        if (surface_id != NULL)
                *surface_id = *(uint32_t *)(output + 0x18);

        return conn;
}


io_connect_t release_surface(io_connect_t surface, uint32_t surface_id)
{
        io_connect_t conn = surface;
        kern_return_t kr = KERN_SUCCESS;

        uint64_t scalar = (uint64_t)surface_id;
        kr = IOConnectCallMethod(conn, 1, &scalar, 1,
                                 NULL, 0,
                                 NULL, NULL, NULL, NULL);
        CHECK_IOKIT_ERR(kr, "release_surface");
        assert(kr == KERN_SUCCESS);

        return conn;
}

void set_indexed_timestamp(io_connect_t c, uint32_t surface_id, uint64_t index, uint64_t value)
{
        uint64_t args[3] = {0};
        args[0] = surface_id;
        args[1] = index;
        args[2] = value;
        kern_return_t kr = IOConnectCallMethod(c, 33, args, 3,
                                               NULL, 0,
                                               NULL, NULL, NULL, NULL);
        CHECK_IOKIT_ERR(kr, "set_indexed_timestamp");
}

kern_return_t iosurface_get_use_count(io_connect_t c, uint32_t surface_id, uint32_t *output)
{
        uint64_t args[1] = {0};
        args[0] = surface_id;
        uint32_t outsize = 1;
        uint64_t out = 0;

        kern_return_t kr = IOConnectCallMethod(c, 16, args, 1,
                                               NULL, 0,
                                               &out, &outsize, NULL, NULL);
        CHECK_IOKIT_ERR(kr, "iosurface_get_use_count");
        *output = (uint32_t)out;
        return kr;
}

