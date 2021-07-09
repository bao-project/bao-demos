#include <config.h>

VM_IMAGE(baremetal_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/baremetal.bin));

struct config config = {
    
    CONFIG_HEADER
    
    .vmlist_size = 1,
    .vmlist = {
        { 
            .image = {
                .base_addr = 0x40200000,
                .load_addr = VM_IMAGE_OFFSET(baremetal_image),
                .size = VM_IMAGE_SIZE(baremetal_image)
            },

            .entry = 0x40200000,

            .platform = {
                .cpu_num = 4,
                
                .region_num = 1,
                .regions =  (struct mem_region[]) {
                    {
                        .base = 0x40200000,
                        .size = 0x1000000 //128MB
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
