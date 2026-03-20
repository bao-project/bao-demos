/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#include <config.h>
#include <plat/platform.h>

#define E3650_UART_BASE         0xF8D60000
#define E3650_GICD_BASE         0xF4000000
#define E3650_GICR_BASE         0xF4100000

#define VM0_IRAM_BASE           0x00B00000
#define VM0_IRAM_SIZE           0x00100000

VM_IMAGE(guest_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/baremetal.bin))

struct config config = {

    .vmlist_size = 1,
    .vmlist = (struct vm_config[]) {
        {   
            .image = VM_IMAGE_BUILTIN(guest_image, VM0_IRAM_BASE),

            .entry = VM0_IRAM_BASE,

            .cpu_affinity = 0xF,

            .platform = {
                .cpu_num = 4,

                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = VM0_IRAM_BASE,
                        .size = VM0_IRAM_SIZE,
                    },
                },

                .dev_num = 1,
                .devs =  (struct vm_dev_region[]) {
                    {
                        .pa = E3650_UART_BASE,
                        .va = E3650_UART_BASE,
                        .size = 0x10000,
                        .interrupt_num = 1,
                        .interrupts = (unsigned[]) { 114 },
                    },
                },

                .arch = {
                    .gic = {
                        .gicd_addr = E3650_GICD_BASE,
                        .gicr_addr = E3650_GICR_BASE,
                    },
                },
            },
        }
    }
};
