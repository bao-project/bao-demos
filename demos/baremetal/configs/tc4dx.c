#include <config.h>

VM_IMAGE(baremetal_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/baremetal.bin))

struct config config = {

        .vmlist_size = 1,
        .hyp.base_addr = 0xA0000000,

        .vmlist = (struct vm_config[]){
            {
                .image = {
                    .base_addr = 0x90100000,
                    .load_addr = VM_IMAGE_OFFSET(baremetal_image),
                    .size = VM_IMAGE_SIZE(baremetal_image),
                },

                .entry = 0x90100000,
                .platform = {
                    .cpu_num = 6,
                    .region_num = 1,
                    .regions = (struct vm_mem_region[]){
                        {
                            .base = 0x90100000,
                            .size = 0x00080000
                        },
                    },
                    .dev_num = 4,
                    .devs = (struct vm_dev_region[]){
                        { //TIMER
                            .pa = 0xF8800000,
                            .va = 0xF8800000,
                            .size = 0x1000,
                            .interrupt_num = 1,
                            .interrupts = (irqid_t[]) {10},
                        },
                        {   //Ports 13
                            .pa = 0xF003D400,
                            .va = 0xF003D400,
                            .size = 0x400,
                        },
                        {   //Ports 14
                            .pa = 0xF003D800,
                            .va = 0xF003D800,
                            .size = 0x400,
                        },
                        { //UART
                            .pa = 0xF46C0000,
                            .va = 0xF46C0000,
                            .size = 0x200,
                            .interrupt_num = 1,
                            .interrupts = (irqid_t[]) {173},
                        },
                    },
                    .arch = {
                        .gpsr_num = 1,
                        .gpsr_groups = (unsigned long int[]){1},
                    }
                }
            },
        },
};
