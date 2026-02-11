/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#include <config.h>

struct config config = {

    .shmemlist_size = 1,
    .shmemlist = (struct shmem[]) {
        [0] = {
            .base = 0x31840000,
            .size = 0x00004000,
        }
    },

    .vmlist_size = 2,
    .vmlist = (struct vm_config[]) {
        {
            //Zephyr VM
            .image = VM_IMAGE_LOADED(0x32300000, 0x32300000, 0x14000),
            .entry = 0x32300000,
            .platform = {
                .cpu_num = 1,
                .region_num = 2,
                .regions =  (struct vm_mem_region[]) {
                    {
                            // CRAM1 (1MiB)
                        .base = 0x32300000,
                        .size = 0x100000
                    },
                    {
                            // DRAM1 (1MiB)
                        .base = 0x31800000,
                        .size = 0x40000
                    }
                },
                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0x31840000,
                        .size = 0x00004000,
                        .shmem_id = 0,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {52}
                    }
                },
                .dev_num = 5,
                    .devs =  (struct vm_dev_region[]) {
                        {
                            /* LINFlexD_9 */
                            .pa = 0x42980000,
                            .va = 0x42980000,
                            .size = 0x10000,
                            .interrupt_num = 1,
                            .interrupts = (irqid_t[]) {253}
                        },
                        {
                            /* SIUL2_5 */
                            .pa = 0x42D20000,
                            .va = 0x42D20000,
                            .size = 0xFFFF
                        },
                        {
                            /* MC_CGM_5 */
                            .pa = 0x42830000,
                            .va = 0x42830000,
                            .size = 0x500
                        },
                        {
                            /* MC_MSCM */
                            .pa = 0x419A0000,
                            .va = 0x419A0000,
                            .size = 0x10000
                        },
                        {
                            // System Timer
                            .interrupt_num = 1,
                            .interrupts = (irqid_t[]) {27}
                        },
                },
                .arch = {
                    .gic = {
                        .gicc_addr = 0x2C000000,
                        .gicd_addr = 0x47800000,
                        .gicr_addr = 0x47900000
                    }
                }
            },
        },
        {
            //Baremetal VM
            .image = VM_IMAGE_LOADED(0x32200000, 0x32200000, 0x14000),
            .entry = 0x32200000,
            .platform = {
                .cpu_num = 3,
                .region_num = 2,
                .regions =  (struct vm_mem_region[]) {
                    {
                        // CRAM1 (1MiB)
                       .base = 0x32200000,
                       .size = 0x100000
                   },
                   {
                        // DRAM1 (1MiB)
                       .base = 0x317C0000,
                       .size = 0x40000
                   }
                },
                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0x31840000,
                        .size = 0x00004000,
                        .shmem_id = 0,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {52}
                    }
                },
                .dev_num = 4,
                .devs =  (struct vm_dev_region[]) {
                    {
                        // LINFlexD_0
                        .pa = 0x40170000,
                        .va = 0x40170000,
                        .size = 0x10000,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {244}
                    },
                    {
                        // SIUL2_0
                        .pa = 0x40520000,
                        .va = 0x40520000,
                        .size = 0x10000
                    },
                    {
                        // MC_CGM_0
                        .pa = 0x40030000,
                        .va = 0x40030000,
                        .size = 0x500
                    },
                    {
                        // System Timer
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {27}
                    },
                },

                .arch = {
                    .gic = {
                        .gicc_addr = 0x2C000000,
                        .gicd_addr = 0x47800000,
                        .gicr_addr = 0x47900000
                    }
                }
            },
        },
    }
};
