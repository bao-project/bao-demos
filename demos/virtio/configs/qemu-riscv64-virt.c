#include <config.h>

VM_IMAGE(linux_backend_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/linux_backend.bin))
VM_IMAGE(freertos_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/freertos.bin))
VM_IMAGE(linux_frontend1_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/linux_frontend1.bin))
VM_IMAGE(linux_frontend2_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/linux_frontend2.bin))

struct config config = {
    
    CONFIG_HEADER
    
    .shmemlist_size = 5,
    .shmemlist = (struct shmem[]) {
        [0] = { .size = 0x01000000, },
        [1] = { .size = 0x01000000, },
        [2] = { .size = 0x01000000, },
        [3] = { .size = 0x01000000, },
        [4] = { .size = 0x01000000, },
    },
    
    .vmlist_size = 4,
    .vmlist = (struct vm_config[]) {
        { 
            .image = {
                .base_addr = 0x90200000,
                .load_addr = VM_IMAGE_OFFSET(linux_backend_image),
                .size = VM_IMAGE_SIZE(linux_backend_image)
            },

            .entry = 0x90200000,
            .cpu_affinity = 0x1,

            .platform = {
                .cpu_num = 1,
                
                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x90000000,
                        .size = 0x40000000,
                        .place_phys = true,
                        .phys = 0x90000000,
                    },
                },
                
                .remio_dev_num = 5,                                                     
                .remio_devs = (struct remio_dev[]) {
                    {
                        .bind_key = 0,
                        .type = REMIO_DEV_BACKEND,
                        .interrupt = 36,                           
                        .shmem = {
                            .base = 0x50000000,
                            .size = 0x01000000,
                            .shmem_id = 0,
                        }
                    },
                    {
                        .bind_key = 1,
                        .type = REMIO_DEV_BACKEND,
                        .interrupt = 37,                           
                        .shmem = {
                            .base = 0x51000000,
                            .size = 0x01000000,
                            .shmem_id = 1,
                        }
                    },
                    {
                        .bind_key = 2,
                        .type = REMIO_DEV_BACKEND,
                        .interrupt = 38,                           
                        .shmem = {
                            .base = 0x52000000,
                            .size = 0x01000000,
                            .shmem_id = 2,
                        }
                    },
                    {
                        .bind_key = 3,
                        .type = REMIO_DEV_BACKEND,
                        .interrupt = 39,                           
                        .shmem = {
                            .base = 0x53000000,
                            .size = 0x01000000,
                            .shmem_id = 3,
                        }
                    },
                    {
                        .bind_key = 4,
                        .type = REMIO_DEV_BACKEND,
                        .interrupt = 40,                           
                        .shmem = {
                            .base = 0x54000000,
                            .size = 0x01000000,
                            .shmem_id = 4,
                        }
                    }
                },

                .dev_num = 1,
                .devs =  (struct vm_dev_region[]) 
                {
                    {
                        .pa = 0x10001000,
                        .va = 0x10001000,
                        .size = 0x8000,                     			            
                        .interrupt_num = 8,                 			        
                        .interrupts = (irqid_t[]) {1, 2, 3, 4, 5, 6, 7, 8}
                    },
                },

                .arch = {
                    .irqc.plic = {
                        .base = 0xC000000,
                    }
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

            .cpu_affinity = 0x2,

            .platform = {
                .cpu_num = 1,
                
                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x0,
                        .size = 0x8000000
                    }
                },

                .remio_dev_num = 1,   
                .remio_devs = (struct remio_dev[]) {
                    {
                        .bind_key = 0,
                        .type = REMIO_DEV_FRONTEND,
                        .size = 0x200,                              
                        .va = 0xa003e00,                            
                        .interrupt = 52,                            
                        .shmem = {
                            .base = 0x50000000,
                            .size = 0x01000000,
                            .shmem_id = 0,
                        }
                    },
                },

                .dev_num = 1,
                .devs =  (struct vm_dev_region[]) {
                    {
                        .pa = 0x10000000,   
                        .va = 0xFF000000,  
                        .size = 0x1000, 
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {10}
                    },
                },

                .arch = {
                    .irqc.plic = {
                        .base = 0xC000000,
                    }
                }
            },
        },
        {
            .image = {
                .base_addr = 0x10200000,
                .load_addr = VM_IMAGE_OFFSET(linux_frontend1_image),
                .size = VM_IMAGE_SIZE(linux_frontend1_image)
            },

            .entry = 0x10200000,
            .cpu_affinity = 0x4,

            .platform = {
                .cpu_num = 1,
                
                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x10000000,
                        .size = 0x40000000,
                    },
                },
                
                .remio_dev_num = 2,
                .remio_devs = (struct remio_dev[]) {
                    {
                        .bind_key = 1,
                        .type = REMIO_DEV_FRONTEND,
                        .size = 0x200,
                        .va = 0xa003e00,
                        .interrupt = 79,
                        .shmem = {
                            .base = 0x51000000,
                            .size = 0x01000000,
                            .shmem_id = 1,
                        }
                    },
                    {
                        .bind_key = 2,
                        .type = REMIO_DEV_FRONTEND,
                        .size = 0x200,
                        .va = 0xa003c00,
                        .interrupt = 78,
                        .shmem = {
                            .base = 0x52000000,
                            .size = 0x01000000,
                            .shmem_id = 2,
                        }
                    }
                },

                .arch = {
                    .irqc.plic = {
                        .base = 0xC000000,
                    }
                }
            },
        },
        {
            .image = {
                .base_addr = 0x10200000,
                .load_addr = VM_IMAGE_OFFSET(linux_frontend2_image),
                .size = VM_IMAGE_SIZE(linux_frontend2_image)
            },

            .entry = 0x10200000,
            .cpu_affinity = 0x8,

            .platform = {
                .cpu_num = 1,
                
                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x10000000,
                        .size = 0x40000000,
                    },
                },
                
                .remio_dev_num = 2,
                .remio_devs = (struct remio_dev[]) {
                    {
                        .bind_key = 3,
                        .type = REMIO_DEV_FRONTEND,
                        .size = 0x200,
                        .va = 0xa003e00,
                        .interrupt = 79,
                        .shmem = {
                            .base = 0x53000000,
                            .size = 0x01000000,
                            .shmem_id = 3,
                        }
                    },
                    {
                        .bind_key = 4,
                        .type = REMIO_DEV_FRONTEND,
                        .size = 0x200, 
                        .va = 0xa003c00,
                        .interrupt = 78,
                        .shmem = {
                            .base = 0x54000000,
                            .size = 0x01000000,
                            .shmem_id = 4,
                        }
                    }
                },

                .arch = {
                    .irqc.plic = {
                        .base = 0xC000000,
                    }
                }
            },
        },
    },
};
