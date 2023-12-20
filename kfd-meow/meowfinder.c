//
//  meowfinder.c
//  meow
//
//  Created by doraaa on 2023/10/15.
//

#include "meowfinder.h"

static unsigned char header[0x4000];

static uint64_t find_prev_insn_kread(uint64_t vaddr, uint32_t num, uint32_t insn, uint32_t mask) {
    uint32_t from = 0;
    while(num) {
        from = kread32(vaddr);
        if((from & mask) == (insn & mask)) {
            return vaddr;
        }
        vaddr -= 4;
        num--;
    }
    return 0;
}

static uint64_t search_proc_set_ucred_kread(uint64_t vaddr, uint64_t size) {
    vaddr += 0x400000; // maybe
    
    for(uint64_t i = 0; i < (size - 0x400000); i += 4) {
        if(kread32(vaddr + i + 0) == 0x910023e3) { // add x3, sp, #0x8
            if(kread32(vaddr + i + 4) == 0x528000a0) { // mov w0, #0x5
                if(kread32(vaddr + i + 8) == 0x52800402) { // mov w2, #0x20
                    if(kread32(vaddr + i + 12) == 0x52800104) { // mov w4, #0x8
                        if((kread32(vaddr + i + 16) & 0xfc000000) == 0x94000000) { // bl _xxx
                            // pongoOS
                            // Most reliable marker of a stack frame seems to be "add x29, sp, 0x...".
                            uint64_t frame = find_prev_insn_kread(vaddr + i, 2000, 0x910003fd, 0xff8003ff);
                            if(frame) {
                                // Now find the insn that decrements sp. This can be either
                                // "stp ..., ..., [sp, -0x...]!" or "sub sp, sp, 0x...".
                                // Match top bit of imm on purpose, since we only want negative offsets.
                                uint64_t start = find_prev_insn_kread(frame, 10, 0xa9a003e0, 0xffe003e0);
                                if(start) {
                                    return start;
                                }
                                else {
                                    start = find_prev_insn_kread(frame, 10, 0xd10003ff, 0xff8003ff);
                                    if(start) {
                                        return start;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

void offsetfinder64_kread(void)
{
    if(!kernel_base) return;
    
    memset(&header, 0, 0x4000);
    kreadbuf(kernel_base, &header, 0x4000);
    
    const struct mach_header_64 *hdr = (struct mach_header_64 *)header;
    const uint8_t *q = NULL;
    
    uint64_t text_exec_addr = 0;
    uint64_t text_exec_size = 0;
    
    uint64_t plk_text_exec_addr = 0;
    uint64_t plk_text_exec_size = 0;
    
    q = header + sizeof(struct mach_header_64);
    for (int i = 0; i < hdr->ncmds; i++) {
        const struct load_command *cmd = (struct load_command *)q;
        if (cmd->cmd == LC_SEGMENT_64) {
            const struct segment_command_64 *seg = (struct segment_command_64 *)q;
            if (!strcmp(seg->segname, "__TEXT_EXEC")) {
                const struct section_64 *sec = (struct section_64 *)(seg + 1);
                for (uint32_t j = 0; j < seg->nsects; j++) {
                    if (!strcmp(sec[j].sectname, "__text")) {
                        text_exec_addr = sec[j].addr;
                        text_exec_size = sec[j].size;
                    }
                }
            }
            
            if (!strcmp(seg->segname, "__PLK_TEXT_EXEC")) {
                const struct section_64 *sec = (struct section_64 *)(seg + 1);
                for (uint32_t j = 0; j < seg->nsects; j++) {
                    if (!strcmp(sec[j].sectname, "__text")) {
                        plk_text_exec_addr = sec[j].addr;
                        plk_text_exec_size = sec[j].size;
                    }
                }
            }
        }
        q = q + cmd->cmdsize;
    }
    
    func__proc_set_ucred = search_proc_set_ucred_kread(text_exec_addr, text_exec_size);
    printf("func__proc_set_ucred : %016llx\n", func__proc_set_ucred);
}
