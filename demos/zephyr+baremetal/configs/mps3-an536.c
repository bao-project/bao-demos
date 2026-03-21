#include <config.h>

struct config config = {

    .shmemlist_size = 1,
    .shmemlist = (struct shmem[]) {
        [0] = { 
            .base = 0x70000000,
            .size = 0x00010000, 
        }
    },

    .vmlist_size = 2,
    .vmlist = (struct vm_config[]) {
        {
            .image = VM_IMAGE_LOADED(0x32200000, 0x32200000, 100 * 1024),
            .entry = 0x32200000,

            .platform = {
                .cpu_num = 1,
                
                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x32200000,
                        .size = 0x04000000,
                    },
                },

                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0x70000000,
                        .size = 0x00010000,
                        .shmem_id = 0,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {52}
                    }
                },

                .dev_num = 2,
                .devs =  (struct vm_dev_region[]) {
                    {   
                        .pa = 0xE0205000,
                        .va = 0xE0205000,
                        .size = 0x40,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {37}  
                    },
                    {   
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {27}  
                    },
                },

                .arch = {
                    .gic = {
                        .gicd_addr = 0xF0000000,
                        .gicr_addr = 0xF0100000,
                    },
                },
            },
        },
        {
            .image = VM_IMAGE_LOADED(0x40000000, 0x40000000, 100 * 1024),

            .entry = 0x40000000,

            .platform = {
                .cpu_num = 1,
                
                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x40000000,
                        .size = 0x10000000,
                    },
                },

                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0x70000000,
                        .size = 0x00010000,
                        .shmem_id = 0,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {52}
                    }
                },

                .dev_num = 2,
                .devs =  (struct vm_dev_region[]) {
                    {   
                        .pa = 0xE0206000,
                        .va = 0xE0206000,
                        .size = 0x1000,
                        .interrupt_num = 2,
                        .interrupts = (irqid_t[]) {39, 40}
                    },
                    {   
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {27}  
                    },
                },

                .arch = {
                    .gic = {
                        .gicd_addr = 0xF0000000,
                        .gicr_addr = 0xF0100000,
                    },
                },
            },
        }
    }
};
