#include <config.h>

VM_IMAGE(baremetal_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/baremetal.bin));
struct config config = {
    
    CONFIG_HEADER
    
    .vmlist_size = 1,
    .vmlist = {
        { 
            .image = {
                .base_addr = 0x0,
                .load_addr = VM_IMAGE_OFFSET(baremetal_image),
                .size = VM_IMAGE_SIZE(baremetal_image)
            },

            .entry = 0x0,

            .platform = {
                .cpu_num = 6,
                
                .region_num = 1,
                .regions =  (struct mem_region[]) {
                    {
                        .base = 0x0,
                        .size = 0x4000000 
                    }
                },

                .dev_num = 2,
                .devs =  (struct dev_region[]) {
                    {   
                        /* uarta */
                        .pa = 0x03100000,
                        .va = 0x80000000,
                        .size = 0x1000,
                        .interrupt_num = 1,
                        .interrupts = (uint64_t[]) {144}                        
                    },
                    {   
                        /* Arch timer interrupt */
                        .interrupt_num = 1,
                        .interrupts = 
                            (uint64_t[]) {27}                         
                    }
                },

                .arch = {
                    .gic = {
                        .gicd_addr = 0xF9010000,
                        .gicc_addr = 0xF9020000,
                    }
                }
            },
        }
    },
};
