#include <config.h>

VM_IMAGE(zephyr_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/zephyr.bin))
VM_IMAGE(baremetal_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/baremetal.bin))

/*
 * Hardware-validated two-VM config: Zephyr on core 0 (DLMU1, 0x90080000),
 * baremetal on cores 1-5 (DLMU2, 0x90100000), talking over a shared-memory
 * IPC channel. DLMU0 (0x90000000) is left free -- it's bao's own
 * PLAT_DATA_ADDR.
 */

struct config config = {

    CONFIG_HEADER

    /* Unchanged from demos/baremetal/configs/tc4dx.c -- Bao itself still
     * loads from PFLASH, leaving all of DLMU/LMU free for the two guests. */
    .hyp.base_addr = 0xA0000000,

    .shmemlist_size = 1,
    .shmemlist = (struct shmem[]) {
        [0] = { 
                .phys = 0x90280000,
                .size = 0x00010000, 
            }
    },

    .vmlist_size = 2,
    .vmlist = (struct vm_config[]){
        { /* Zephyr -- DLMU1 */
            .image = {
                .base_addr = 0x90080000,
                .load_addr = VM_IMAGE_OFFSET(zephyr_image),
                .size = VM_IMAGE_SIZE(zephyr_image),
            },

            .entry = 0x90080000,
            .cpu_affinity = 0x1, /* core 0 */

            .platform = {
                .cpu_num = 1,
                .region_num = 1,
                .regions = (struct vm_mem_region[]){
                    {
                        .base = 0x90080000,
                        .size = 0x80000,
                    },
                },

                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0x90280000,
                        .size = 0x00010000,
                        .shmem_id = 0,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {1360},
                    }
                },

                .dev_num = 4,
                .devs = (struct vm_dev_region[]){
                    { //TIMER (core-0 STM; matches this board's default core)
                        .pa = 0xF8800000,
                        .va = 0xF8800000,
                        .size = 0x1000,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {10},
                    },
                    {   //Ports 13 -- ASCLIN0 pin mux
                        .pa = 0xF003D400,
                        .va = 0xF003D400,
                        .size = 0x400,
                    },
                    {   //Ports 14 -- ASCLIN0 pin mux
                        .pa = 0xF003D800,
                        .va = 0xF003D800,
                        .size = 0x400,
                    },
                    { //UART -- ASCLIN0 (0xF46C0000), Tx/Rx/Err
                        .pa = 0xF46C0000,
                        .va = 0xF46C0000,
                        .size = 0x200,
                        .interrupt_num = 3,
                        .interrupts = (irqid_t[]) {172, 173, 174},
                    },
                },

                .arch = {
                    .gpsr_num = 1,
                    .gpsr_groups = (unsigned long int[]){2},
                }
            }
        },
        { /* Baremetal -- DLMU2, cores 1-5, ASCLIN1 (Zephyr owns ASCLIN0).
           * Build with TC4DX_ASCLIN=1. */
            .image = {
                .base_addr = 0x90100000,
                .load_addr = VM_IMAGE_OFFSET(baremetal_image),
                .size = VM_IMAGE_SIZE(baremetal_image),
            },

            .entry = 0x90100000,
            .cpu_affinity = 0x3E, /* cores 1-5 */

            .platform = {
                .cpu_num = 5,
                .region_num = 1,
                .regions = (struct vm_mem_region[]){
                    {
                        .base = 0x90100000,
                        .size = 0x80000,
                    },
                },

                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0x90280000,
                        .size = 0x00010000,
                        .shmem_id = 0,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {1361},
                    }
                },

                .dev_num = 3,
                .devs = (struct vm_dev_region[]){
                    { //TIMER -- CPU1.SFR (master CPU 1); IRQ 28 = STMCPU1_SR4,
                      //this VM's own compare-match line, confirmed on
                      //hardware -- not 26 (SR2), which belongs to Zephyr.
                        .pa = 0xF8840000,
                        .va = 0xF8840000,
                        .size = 0x1000,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {28},
                    },
                    {   //Port 33 -- ASCLIN1 pin mux
                        .pa = 0xF0042400,
                        .va = 0xF0042400,
                        .size = 0x400,
                    },
                    { //UART -- ASCLIN1 (0xF46C0200), Tx/Rx/Err
                        .pa = 0xF46C0200,
                        .va = 0xF46C0200,
                        .size = 0x200,
                        .interrupt_num = 3,
                        .interrupts = (irqid_t[]) {175, 176, 177},
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
