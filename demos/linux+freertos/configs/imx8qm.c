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
                .base_addr = 0x80200000,
                .load_addr = VM_IMAGE_OFFSET(linux_image),
                .size = VM_IMAGE_SIZE(linux_image)
            },

            .entry = 0x80200000,
            .cpu_affinity = 0b110111,

            .platform = {
                .cpu_num = 5,   
                
                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x80200000,
                        .size = 0x20000000
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

/**
 * We are assigning a MU to the linux guest because the linux drivers 
 * assume linux can directly interact with the SCU to configure its devices.
 * Therefore, this guest will be able to configure peripheral not assigned
 * to it, as for example, the lpuart0 used by bao and freertos. In the future 
 * we will have to either move the cores and peripherals belonging to this guest
 * to a parate scfw partition or paravirtualise the MUS and interpose the guest 
 * communication to the SCU to limit which resources it might configure.
 */ 
                .dev_num = 4,
                .devs =  (struct vm_dev_region[]) {  
                    {
                        /* lsio_mu1 message unit */
                        .pa = 0x5d1c0000,
                        .va = 0x5d1c0000,
                        .size = 0x10000,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {209},    
                    },
                    {   
                        /* Arch timer interrupt */
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {27}                         
                    },
                    {   
                        /* enet0 clock generator */
                        .pa = 0x5b230000,
                        .va = 0x5b230000,
                        .size = 0x10000,
                    },
                    {   
                        /* fec1 */
                        .pa = 0x5b040000,
                        .va = 0x5b040000,
                        .size = 0x10000,
                        .interrupt_num = 4,
                        .interrupts = (irqid_t[]) {288, 289, 290, 291},    
                        .id = 0x2,
                    },
                },

                .arch = {
                    .gic = {
                       .gicd_addr = 0x51a00000,
                       .gicr_addr = 0x51b00000
                    },
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
            .cpu_affinity = 0b001000,

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

                .dev_num = 2,
                .devs =  (struct vm_dev_region[]) {
                    {   
                        /* lpuart0 */
                        .pa = 0x5a060000,
                        .va = 0xff000000,
                        .size = 0x1000,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {377}                           
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
                        .gicd_addr = 0xF9010000,
                        .gicr_addr = 0xF9020000,
                    }
                }
            },
        }
    },
};
