include $(bao_demos)/guests/freertos/make.mk
include $(bao_demos)/guests/baremetal/make.mk

freertos_image:=$(wrkdir_demo_imgs)/freertos.bin
freertos_args:=APP_SRC_DIR=$(bao_demos)/demos/freertos+baremetal/freertos-app

ifeq ($(PLATFORM),rh850-u2a16)
freertos_args+=RO_MEM_BASE=0x100000 RO_MEM_SIZE=0x100000 \
				RW_MEM_BASE=0xfe100000 RW_MEM_SIZE=0x80000
freertos_args+=SHMEM_BASE=0xfe480000 SHMEM_SIZE=0x1000
endif

$(eval $(call build-freertos, $(freertos_image), $(freertos_args)))

baremetal_image:=$(wrkdir_demo_imgs)/baremetal.bin
baremetal_args:=APP_SRC_DIR=$(bao_demos)/demos/freertos+baremetal/baremetal-app

ifeq ($(PLATFORM),rh850-u2a16)
baremetal_args+=RO_MEM_BASE=0x200000 RO_MEM_SIZE=0x100000 \
				RW_MEM_BASE=0xfe400000 RW_MEM_SIZE=0x80000
baremetal_args+=SHMEM_BASE=0xfe480000 SHMEM_SIZE=0x1000
$(baremetal_image): export CPPFLAGS+=\
    -DPLAT_UART_ADDR=0xFFD28400UL \
    -DUART_IRQ_ID=434UL \
    -DPLAT_OSTM_BASE=0xFFBF0100UL \
    -DTIMER_IRQ_ID=200UL
endif

$(eval $(call build-baremetal, $(baremetal_image), $(baremetal_args)))

guest_images:=$(freertos_image) $(baremetal_image)