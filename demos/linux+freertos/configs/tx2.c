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
                .base_addr = 0x90000000,
                .load_addr = VM_IMAGE_OFFSET(linux_image),
                .size = VM_IMAGE_SIZE(linux_image)
            },

            .entry = 0x90000000,
            .cpu_affinity = 0b111110,

            .platform = {
                .cpu_num = 5,
                
                .region_num = 2,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x90000000,
                        .size = 0x40000000,                                     
                    },
                    {   
                        /* sysram */
                        .base = 0x30000000,
                        .size = 0x50000,
                        .place_phys = true,
                        .phys = 0x30000000
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

                .dev_num = 5,
                .devs =  (struct vm_dev_region[]) {
                    {   
                        /* ethernet */
                        .pa = 0x02490000,
                        .va = 0x02490000,
                        .size = 0x10000,
                        .interrupt_num = 10,
                        .interrupts = (irqid_t[]) {226,227,222,218,223,219,224,220,225,221},
                        .id = 0x1                  
                    },
                    {
                        /* mailbox hs0 */
                        .pa = 0x03c00000,
                        .va = 0x03c00000,
                        .size = 0xa0000,    
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {208},
                    },
                    {
                        /* bpmp */
                        .id = 0x32
                    },
                    {
                        /* gpio */
                        .pa = 0x2200000,
                        .va = 0x2200000,
                        .size = 0x20000,    
                        .interrupt_num = 6,
                        .interrupts = (irqid_t[]) {79,82,85,88,91,212}
                    },
                    {   
                        /* Arch timer interrupt */
                        .interrupt_num = 1,
                        .interrupts = 
                            (irqid_t[]) {27}                         
                    }
                },

                .arch = {
                    .gic = {
                        .gicd_addr = 0x03881000,
                        .gicc_addr = 0x03882000,
                    }
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
                        /* uarta */
                        .pa = 0x03100000,
                        .va = 0xff000000,
                        .size = 0x1000,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {144}                        
                    },
                    {   
                        /* Arch timer interrupt */
                        .interrupt_num = 1,
                        .interrupts = 
                            (irqid_t[]) {27}                         
                    }
               },

                .arch = {
                    .gic = {
                        .gicd_addr = 0xf9010000,
                        .gicc_addr = 0xf9020000,
                    }
                }
            },
        },
    },
};
