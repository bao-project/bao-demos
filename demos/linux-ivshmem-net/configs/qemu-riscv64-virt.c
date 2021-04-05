
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
                .base_addr = 0x80200000,
                .load_addr = VM_IMAGE_OFFSET(linux1),
                .size = VM_IMAGE_SIZE(linux1)
            },

            .entry = 0x80200000,

            .platform = {
                .cpu_num = 1,
                
                .region_num = 1,
                .regions =  (struct mem_region[]) {
                    {
                        .base = 0x80000000,
                        .size = 0x40000000
                    }
                },

                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0xf0000000,
                        .size = 0x00010000,
                        .shmem_id = 0,
                        .interrupt_num = 2,
                        .interrupts = (uint64_t[]) {52, 53}
                    }
                },

                .dev_num = 1,
                .devs =  (struct dev_region[]) {
                    {
                        /* UART */
                        .pa = 0x10000000,   
                        .va = 0x10000000,  
                        .size = 0x1000,
                        .interrupt_num = 1,
                        .interrupts = (uint64_t[]) {10}
                    },
                },

                .arch = {
                   .plic_base = 0xc000000,
                }
            },
        },
        { 
            .image = {
                .base_addr = 0xa0200000,
                .load_addr = VM_IMAGE_OFFSET(linux2),
                .size = VM_IMAGE_SIZE(linux2)
            },

            .entry = 0xa0200000,

            .platform = {
                .cpu_num = 3,
                
                .region_num = 1,
                .regions =  (struct mem_region[]) {
                    {
                        .base = 0xa0000000,
                        .size = 0x40000000, //128MB
                        .place_phys = true,
                        .phys = 0xa0000000,
                    }
                },

                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0xf0000000,
                        .size = 0x00010000,
                        .shmem_id = 0,
                        .interrupt_num = 2,
                        .interrupts = (uint64_t[]) {52, 53}
                    }
                },

                .dev_num = 1,
                .devs =  (struct dev_region[]) {
                    {
                        /* virtio devices */
                        .pa = 0x10001000,   
                        .va = 0x10001000,  
                        .size = 0x8000, 
                        .interrupt_num = 8,
                        .interrupts = (uint64_t[]) {1,2,3,4,5,6,7,8}
                    },
                },

                .arch = {
                   .plic_base = 0xc000000,
                }
            },
        },
    }
};
