#include <config.h>

struct config config = {
    
    .vmlist_size = 1,
    .vmlist = (struct vm_config[]) {
        { 
            .image = VM_IMAGE_LOADED(0x32200000, 0x32200000, 100*1024),
            .entry = 0x32200000,

            .platform = {
                .cpu_num = 2,
                
                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x32200000,
                        .size = 0x4000000 
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
                    }
                }
            },
        }
    },
};
