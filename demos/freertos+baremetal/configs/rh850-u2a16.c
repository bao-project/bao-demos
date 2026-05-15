/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#include <config.h>

struct config config = {

    CONFIG_HEADER

    .shmemlist_size = 1,
    .shmemlist = (struct shmem[]) {
        // Cluster2 RAM
        [0] = { 
            .base = 0xfe480000,
            .size = 0x1000
        }
    },

    .vmlist_size = 2,

    .vmlist = (struct vm_config[]){
        // VM1: FreeRTOS
        {
            .entry = 0x100000,
            .image = VM_IMAGE_LOADED(0x100000,0x100000,0x100000),
            // .cpu_affinity = 0x1,

            .platform = {
                .cpu_num = 1,

                .region_num = 2,
                .regions =  (struct vm_mem_region[]) {
                    // Code Flash (Bank B) - Code
                    {
                        .base = 0x100000,
                        .size = 0x100000
                    },
                    // Cluster1 RAM - Data
                    {
                        .base = 0xfe100000,
                        .size = 0x80000
                    }
                },

                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0xfe480000,
                        .size = 0x1000,
                        .shmem_id = 0,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {32}
                    }
                },

                .dev_num = 4,
                .devs =  (struct vm_dev_region[]) {
                    // Standby Controller
                    {
                        // 0xFF981000 -> 0xFF982000
                        .id = 0,
                        .pa = 0xFF981000,
                        .va = 0xFF981000,
                        .size = 0x1000,
                        .interrupt_num = 0,
                        .interrupts = NULL
                    },
                    // RLIN35
                    {
                        // 0xFFC7C100 -> 0xFFC7C140
                        .id = 0,
                        .pa = 0xFFC7C100,
                        .va = 0xFFC7C100,
                        .size = 0x40,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {438}
                    },
                    // OSTM0
                    {
                        // 0xFFBF0000 -> 0xFFBF0100
                        .id = 0,
                        .pa = 0xFFBF0000,
                        .va = 0xFFBF0000,
                        .size = 0x100,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {199}
                    },
                    // INTC1 (self)
                    {
                        // 0xFFFC0000 -> 0xFFFC4000
                        .id = 0,
                        .pa = 0xFFFC0000,
                        .va = 0xFFFC0000,
                        .size = 0x4000,
                        .interrupt_num = 0,
                        .interrupts = NULL
                    }
                },
            }
        },

        // VM2: Baremetal
        {
            .entry = 0x200000,
            .image = VM_IMAGE_LOADED(0x200000,0x200000,0x100000),
            // .cpu_affinity = 0xE,

            .platform = {
                .cpu_num = 3,

                .region_num = 2,
                .regions =  (struct vm_mem_region[]) {
                    // Code Flash (Bank B) - Code
                    {
                        .base = 0x200000,
                        .size = 0x100000
                    },
                    // Cluster1 RAM - Data
                    {
                        .base = 0xfe400000,
                        .size = 0x80000
                    }
                },

                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0xfe480000,
                        .size = 0x1000,
                        .shmem_id = 0,
                        .interrupt_num = 0,
                        .interrupts = NULL
                    }
                },

                .dev_num = 4,
                .devs =  (struct vm_dev_region[]) {
                    // Standby Controller
                    {
                        // 0xFF981000 -> 0xFF982000
                        .id = 0,
                        .pa = 0xFF981000,
                        .va = 0xFF981000,
                        .size = 0x1000,
                        .interrupt_num = 0,
                        .interrupts = NULL
                    },
                    // RLIN34
                    {
                        // 0xFFD28400 -> 0xFFD28440
                        .id = 0,
                        .pa = 0xFFD28400,
                        .va = 0xFFD28400,
                        .size = 0x40,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {434}
                    },
                    // OSTM1
                    {
                        // 0xFFBF0100 -> 0xFFBF0200
                        .id = 0,
                        .pa = 0xFFBF0100,
                        .va = 0xFFBF0100,
                        .size = 0x100,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {200}
                    },
                    // INTC1 (self)
                    {
                        // 0xFFFC0000 -> 0xFFFC4000
                        .id = 0,
                        .pa = 0xFFFC0000,
                        .va = 0xFFFC0000,
                        .size = 0x4000,
                        .interrupt_num = 0,
                        .interrupts = NULL
                    }
                },
            }
        }
    }
};