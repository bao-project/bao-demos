#include <config.h>

VM_IMAGE(linux_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/linux.bin));
VM_IMAGE(freertos_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/freertos.bin));

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
                .base_addr = 0x40200000,
                .load_addr = VM_IMAGE_OFFSET(linux_image),
                .size = VM_IMAGE_SIZE(linux_image)
            },

            .entry = 0x40200000,

            .platform = {
                .cpu_num = 3,
                
                .region_num = 1,
                .regions =  (struct mem_region[]) {
                    {
                        .base = 0x40200000,
                        .size = 0x10000000
                    }
                },

                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0xf0000000,
                        .size = 0x00010000,
                        .shmem_id = 0,
                        .interrupt_num = 1,
                        .interrupts = (uint64_t[]) {52}
                    }
                },

                .dev_num = 1,
                .devs =  (struct dev_region[]) {
                    {
                        .pa = 0xff010000,   
                        .va = 0xff010000,  
                        .size = 0x1000, 
                        .interrupt_num = 1,
                        .interrupts = (uint64_t[]) {2}
                    },
                },

                .arch = {
                   .plic_base = 0xc000000,
                }
            },
        },
        { 
            .image = {
                .base_addr = 0x40200000,
                .load_addr = VM_IMAGE_OFFSET(freertos_image),
                .size = VM_IMAGE_SIZE(freertos_image)
            },

            .entry = 0x40200000,

            .platform = {
                .cpu_num = 1,
                
                .region_num = 1,
                .regions =  (struct mem_region[]) {
                    {
                        .base = 0x40200000,
                        .size = 0x01000000
                    }
                },

                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0x70000000,
                        .size = 0x00010000,
                        .shmem_id = 0,
                        .interrupt_num = 1,
                        .interrupts = (uint64_t[]) {52}
                    }
                },

                .dev_num = 1,
                .devs =  (struct dev_region[]) {
                    {
                        .pa = 0xff000000,   
                        .va = 0xff000000,  
                        .size = 0x1000, 
                        .interrupt_num = 1,
                        .interrupts = (uint64_t[]) {1}
                    },
                },

                .arch = {
                   .plic_base = 0xc000000,
                }
            },
        },

     }
};
