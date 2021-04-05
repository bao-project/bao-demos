#include <config.h>

VM_IMAGE(linux1, XSTR(BAO_DEMOS_WRKDIR_IMGS/linux1.bin));
VM_IMAGE(linux2, XSTR(BAO_DEMOS_WRKDIR_IMGS/linux2.bin));

struct config config = {
    
    CONFIG_HEADER

    .shmemlist_size = 1,
    .shmemlist = (struct shmem[]) {
        [0] = { .size = 0x00010000, }
    },

    .vmlist_size = 2,
    .vmlist = {
        { 
            .image = {
                .base_addr = 0x40000000,
                .load_addr = VM_IMAGE_OFFSET(linux1),
                .size = VM_IMAGE_SIZE(linux1)
            },

            .entry = 0x40000000,
            .cpu_affinity = 0b1,

            .platform = {
                .cpu_num = 1,
                
                .region_num = 1,
                .regions =  (struct mem_region[]) {
                    {
                        .base = 0x40000000,
                        .size = 0x20000000,
                    }
                },

                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0x60000000,
                        .size = 0x00010000,
                        .shmem_id = 0,
                        .interrupt_num = 2,
                        .interrupts = (uint64_t[]) {184,185}
                    }
                },

                .dev_num = 2,
                .devs =  (struct dev_region[]) {
                    {   
                        /* PL011 */
                        .pa = 0x9000000,
                        .va = 0x9000000,
                        .size = 0x1000,
                        .interrupt_num = 1,
                        .interrupts =  (uint64_t[]) {33}                           
                    },
                    {   
                        /* Arch timer interrupt */
                        .interrupt_num = 1,
                        .interrupts = (uint64_t[]) {27}                         
                    }
                },

                .arch = {
                    .gic = {
                       .gicd_addr = 0x8000000,
                       .gicr_addr = 0x80A0000
                    }
                }
            },
        },
        { 
            .image = {
                .base_addr = 0x40000000,
                .load_addr = VM_IMAGE_OFFSET(linux2),
                .size = VM_IMAGE_SIZE(linux2)
            },

            .entry = 0x40000000,
            .cpu_affinity = 0b1110,

            .platform = {
                .cpu_num = 3,
                
                .region_num = 1,
                .regions =  (struct mem_region[]) {
                    {
                        .base = 0x40000000,
                        .size = 0x20000000
                    }
                },

                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0x60000000,
                        .size = 0x00010000,
                        .shmem_id = 0,
                        .interrupt_num = 2,
                        .interrupts = (uint64_t[]) {184,185}
                    }
                },

                .dev_num = 1,
                .devs =  (struct dev_region[]) {
                    {   
                        /* Arch timer interrupt */
                        .interrupt_num = 1,
                        .interrupts = 
                            (uint64_t[]) {27}                         
                    }
                },

                .arch = {
                    .gic = {
                       .gicd_addr = 0x8000000,
                       .gicr_addr = 0x80A0000
                    }
                }
            },
        }
    },
};
