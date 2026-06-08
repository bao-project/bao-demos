#include <config.h>

VM_IMAGE(baremetal_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/baremetal.bin))

struct config config = {

    .vmlist_size = 1,
    .vmlist = (struct vm_config[]) {
        {
            .image = {
                .base_addr = 0x102000000,
                .load_addr = VM_IMAGE_OFFSET(baremetal_image),
                .size = VM_IMAGE_SIZE(baremetal_image)
            },

            .entry = 0x102000000,

            .platform = {
                .cpu_num = 8,

                .region_num = 1,
                .regions = (struct vm_mem_region[]) {
                    {
                        .base = 0x102000000,
                        .size = 0x4000000
                    }
                },

                .dev_num = 1,
                .devs = (struct vm_dev_region[]) {
                    {
                        /* K3 UART0 passthrough */
                        .pa = 0xd4017000,
                        .va = 0xd4017000,
                        .size = 0x1000,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) { 42 }
                    },
                },

                .arch = {
                    .irqc.aia.aplic.base = 0xe0804000,
                    .irqc.aia.imsic.base = 0xe0400000,
                }
            },
        },
    }
};
