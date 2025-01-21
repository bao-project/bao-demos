
freertos_image:=$(wrkdir_demo_imgs)/freertos.bin
make_args:=STD_ADDR_SPACE=y
make_args+=APP_SRC_DIR=$(bao_demos)/demos/torizonos+freertos/freertos-app
make_args+=SHMEM_BASE=0xf0000000 SHMEM_SIZE=0x200000
$(eval $(call build-freertos, $(freertos_image), $(make_args)))

include $(bao_demos)/guests/freertos/make.mk
$(eval $(call build-freertos, $(freertos_image), $(make_args)))

include $(bao_demos)/guests/torizonos/make.mk

guest_images:=torizonos_images $(freertos_image)
