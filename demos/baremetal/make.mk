include $(bao_demos)/guests/baremetal/make.mk

baremetal_image:=$(wrkdir_demo_imgs)/baremetal.bin
$(eval $(call build-baremetal, $(baremetal_image)))

guest_images:=$(baremetal_image)