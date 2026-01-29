/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#include <config.h>

struct config config = {

    CONFIG_HEADER

    .vmlist_size = 1,

    .vmlist = (struct vm_config[]){
        {
            .entry = 0x7F0000,
            .image = VM_IMAGE_LOADED(0x7F0000,0x7F0000,0x10000),
            .cpu_affinity = 0xf,

            .platform = {
                .cpu_num = 4,
                .region_num = 2,
                .regions =  (struct vm_mem_region[]) {
                    // Code Flash (Bank A) -> Guest code
                    {
                        .base = 0x7F0000,
                        .size = 0x10000
                    },
                    // Cluster1 RAM -> Guest Data
                    {
                        .base = 0xfe100000,
                        .size = 0x80000
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
        }
    }
};
