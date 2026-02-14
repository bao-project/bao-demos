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
            .entry = 0x80000000,
            .image = {
                .base_addr = 0x80000000,
                .load_addr = VM_IMAGE_OFFSET(linux_image),
                .size = VM_IMAGE_SIZE(linux_image)
            },
            .cpu_affinity = 0xFE,

            .platform = {
                .cpu_num = 7,

                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x80000000,
                        .size = 0x68000000,
                        .place_phys = true,
                        .phys = 0x80000000,
                    },
                },

                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0xF0000000,
                        .size = 0x00010000,
                        .shmem_id = 0,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {52}
                    }
                },

                .dev_num = 7,
                .devs =  (struct vm_dev_region[]) { 
                    // GMAC_0 (ethernet0)
                    {
                        .pa = 0x4033c000,
                        .va = 0x4033c000,
                        .size = 0x3000,
                        .interrupt_num = 11,
                        .interrupts = (irqid_t[]) {89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99},  
                    },
                    // PCIe APB (serdes0)
                    {
                        .pa = 0x40480000,
                        .va = 0x40480000,
                        .size = 0x100000,
                    }, 
                    // GPR
                    { 
                        .pa = 0x4007c000,
                        .va = 0x4007c000,
                        .size = 0x3000,
                    },
                    // SIUL2_0
                    { 
                        .pa = 0x4009c000,
                        .va = 0x4009c000,
                        .size = 0x5000,
                    },
                    // SIUL2
                    {
                        .pa = 0x44010000,
                        .va = 0x44010000,
                        .size = 0x5000,
                    },
                    // SCMI
                    { 
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {332},        
                    },
                    // Arch timer
                    {   
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {27}                         
                    },
                },

                .arch = {
                    .gic = {
                        .gicd_addr = 0x50800000,
                        .gicr_addr = 0x50900000,
                    },
                    .smc_passthrough = {
                        .num = 3,
                        .smcs = (size_t[]) {0xc20000fe, 0x80000000, 0x80000001}
                    }
                }
            }
        },
        {
            .image = {
                .base_addr = 0x0,
                .load_addr = VM_IMAGE_OFFSET(freertos_image),
                .size = VM_IMAGE_SIZE(freertos_image)
            },

            .entry = 0x0,
            .cpu_affinity = 0x01,

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
                        .base = 0xF0000000,
                        .size = 0x00010000,
                        .shmem_id = 0,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {52}
                    }
                },

                .dev_num = 2,
                .devs =  (struct vm_dev_region[]) {
                    // serial0
                    { 
                        .pa = 0x401c8000,
                        .va = 0x401c8000,
                        .size = 0x3000,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {114},        
                    },
                    {   
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {27}                         
                    }
               },

                .arch = {
                    .gic = {
                        .gicd_addr = 0xf9010000,
                        .gicr_addr = 0xf9020000,
                    }
                }
            },
        },
    },
};
