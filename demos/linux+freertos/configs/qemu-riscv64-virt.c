#include <config.h>

VM_IMAGE(linux_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/linux.bin))
VM_IMAGE(freertos_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/freertos.bin))

struct config config = {
    
    CONFIG_HEADER

    .shmemlist_size = 1,
    .shmemlist = (struct shmem[]) {
        [0] = { .size = 0x00010000, }
    },
    
    .vmlist_size = 2,
    .vmlist = (struct vm_config[]) {
        { 
            .image = {
                .base_addr = 0x90200000,
                .load_addr = VM_IMAGE_OFFSET(linux_image),
                .size = VM_IMAGE_SIZE(linux_image)
            },

            .entry = 0x90200000,

            .platform = {
                .cpu_num = 3,
                
                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x90000000,
                        .size = 0x40000000,
                        .place_phys = true,
                        .phys = 0x90000000
                    }
                },

                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0xf0000000,
                        .size = 0x00010000,
                        .shmem_id = 0,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {52}
                    }
                },

                .dev_num = 1,
                .devs =  (struct vm_dev_region[]) {
                    {
                        /* virtio devices */
                        .pa = 0x10001000,   
                        .va = 0x10001000,  
                        .size = 0x8000, 
                        .interrupt_num = 8,
                        .interrupts = (irqid_t[]) {1,2,3,4,5,6,7,8}
                    },
                },

                .arch = {
                    .irqc.plic.base = 0xc000000,
                }
            },
        },
        { 
            .image = {
                .base_addr = 0x0,
                .load_addr = VM_IMAGE_OFFSET(freertos_image),
                .size = VM_IMAGE_SIZE(freertos_image)
            },

            .entry = 0x0,

            .platform = {
                .cpu_num = 1,
                
                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x0,
                        .size = 0x8000000
                    }
                },

                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0xf0000000,
                        .size = 0x00010000,
                        .shmem_id = 0,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {52}
                    }
                },

                .dev_num = 1,
                .devs =  (struct vm_dev_region[]) {
                    {
                        .pa = 0x10000000,   
                        .va = 0xff000000,  
                        .size = 0x1000, 
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {10}
                    },
                },

                .arch = {
                    .irqc.plic.base = 0xc000000,
                }
            },
        },
    }
};
