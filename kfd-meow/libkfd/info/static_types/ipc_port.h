/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef ipc_port_h
#define ipc_port_h

struct ipc_port {
    struct {
        uint32_t io_bits;
        uint32_t io_references;
    } ip_object;
    uint64_t ip_waitq_and_ip_messages[7];
    union {
        uint64_t ip_receiver;
        uint64_t ip_destination;
        uint32_t ip_timestamp;
    };
    union {
        uint64_t ip_kobject;
        uint64_t ip_imp_task;
        uint64_t ip_sync_inheritor_port;
        uint64_t ip_sync_inheritor_knote;
        uint64_t ip_sync_inheritor_ts;
    };
    union {
        int32_t ip_pid;
        uint64_t ip_twe;
        uint64_t ip_pdrequest;
    };
    uint64_t ip_nsrequest;
    uint64_t ip_requests;
    union {
        uint64_t ip_premsg;
        uint64_t ip_send_turnstile;
    };
    uint64_t ip_context;
    uint32_t ip_impcount;
    uint32_t ip_mscount;
    uint32_t ip_srights;
    uint32_t ip_sorights;
    union {
        uint64_t ip_kolabel;
        uint64_t ip_splabel;
    };
};

#endif /* ipc_port_h */
