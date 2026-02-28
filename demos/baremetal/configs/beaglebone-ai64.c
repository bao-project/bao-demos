#include <config.h>

VM_IMAGE(baremetal_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/baremetal.bin))

struct config config = {

    CONFIG_HEADER

        .vmlist_size = 1,
    .vmlist =
        (struct vm_config[]){
            {
                .image =
                    {
                        .base_addr = 0x80000000,
                        .load_addr = VM_IMAGE_OFFSET(baremetal_image),
                        .size = VM_IMAGE_SIZE(baremetal_image),
                    },

                .entry = 0x80000000,

                .cpu_affinity = (1UL << 0),

                .platform =
                    {
                        .cpu_num = 1,

                        .region_num = 1,
                        .regions =
                            (struct vm_mem_region[]){
                                {
                                    .base = 0x80000000,
                                    .size = 0x8000000,
                                },
                            },

                        .dev_num = 2,
                        .devs =
                            (struct vm_dev_region[]){
                                {
                                    /* MAIN_UART0 */
                                    .pa = 0x02800000,
                                    .va = 0x02800000,
                                    .size = 0x1000,
                                    .interrupt_num = 1,
                                    .interrupts =
                                        (irqid_t[]){224} /* SPI 192 -> 192
                                                            + 32 = 224 */
                                },
                                {
                                    /* Arch timer interrupt */
                                    .interrupt_num = 1,
                                    .interrupts = (irqid_t[]){27},
                                },
                            },

                        .arch =
                            {
                                .gic =
                                    {
                                        .gicd_addr = 0x01800000,
                                        .gicr_addr = 0x01900000,
                                    },
                            },
                    },
            },
        },
};
