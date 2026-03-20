include $(bao_demos)/guests/linux/make.mk
include $(bao_demos)/guests/freertos/make.mk

linux_frontend1_image=$(wrkdir_demo_imgs)/linux_frontend1.bin
linux_frontend2_image=$(wrkdir_demo_imgs)/linux_frontend2.bin

linux_frontend1_dts=$(bao_demos)/demos/$(DEMO)/devicetrees/$(PLATFORM)/linux-frontend1.dts
linux_frontend2_dts=$(bao_demos)/demos/$(DEMO)/devicetrees/$(PLATFORM)/linux-frontend2.dts

export PATH:=$(BAO_DEMOS_BUILDROOT)/output/host/bin/:${PATH}

$(eval $(call build-linux, $(linux_frontend1_image), $(linux_frontend1_dts)))
$(eval $(call build-linux, $(linux_frontend2_image), $(linux_frontend2_dts)))

freertos_image:=$(wrkdir_demo_imgs)/freertos.bin
make_args:=APP_SRC_DIR=$(bao_demos)/demos/virtio/freertos-app
make_args+=STD_ADDR_SPACE=y SHMEM_BASE=0xD0000000 SHMEM_SIZE=0x1000000

DEMO_FILE := $(bao_demos)/demos/virtio/freertos-app/demo.c

$(eval $(call build-freertos, $(freertos_image), $(make_args)))

export BAO_DEMOS_BUILDROOT_POST_BUILD_SCRIPT=$(bao_demos)/demos/$(DEMO)/setup/post-build.sh

bao_virtio_setup_files:=$(bao_demos)/demos/$(DEMO)/setup
bao_virtio_frontend1_image:=$(wrkdir_demo_imgs)/ext4_frontend_vm1_image.img
bao_virtio_frontend2_image:=$(wrkdir_demo_imgs)/ext4_frontend_vm2_image.img
bao_virtio_frontend1_mp:=$(wrkdir_src)/ext4_frontend_vm1
bao_virtio_frontend2_mp:=$(wrkdir_src)/ext4_frontend_vm2

export BAO_DEMOS_FRONTEND1_IMG=$(bao_virtio_frontend1_image)
export BAO_DEMOS_FRONTEND2_IMG=$(bao_virtio_frontend2_image)
export BAO_DEMOS_FRONTEND1_MP=$(bao_virtio_frontend1_mp)
export BAO_DEMOS_FRONTEND2_MP=$(bao_virtio_frontend2_mp)

# Target to prepare Linux frontend disk images & backend overlay
prepare-virtio:
	@echo "This operation requires sudo privileges to perform tasks on disk images."
	@echo "1. Creating two 1MB disk images for the Linux frontends."
	@echo "2. Formatting them as ext4."
	@echo "3. Mounting to add test data."
	@echo "4. Unmounting after operations."

	# Create disk images
	dd if=/dev/zero of=$(bao_virtio_frontend1_image) bs=1M count=1
	dd if=/dev/zero of=$(bao_virtio_frontend2_image) bs=1M count=1

	# Format disk images
	@echo "Formatting VM1 disk image..."
	sudo mkfs.ext4 -F $(bao_virtio_frontend1_image)

	@echo "Formatting VM2 disk image..."
	sudo mkfs.ext4 -F $(bao_virtio_frontend2_image)

	# Mount, add test data, unmount
	mkdir -p $(bao_virtio_frontend1_mp)
	@echo "Mounting VM1 disk image..."
	sudo mount -o loop $(bao_virtio_frontend1_image) $(bao_virtio_frontend1_mp)
	echo "Hello from VM1" | sudo tee $(bao_virtio_frontend1_mp)/hello.txt > /dev/null
	@echo "Unmounting VM1..."
	sync
	sudo umount --lazy $(bao_virtio_frontend1_mp)

	mkdir -p $(bao_virtio_frontend2_mp)
	@echo "Mounting VM2 disk image..."
	sudo mount -o loop $(bao_virtio_frontend2_image) $(bao_virtio_frontend2_mp)
	echo "Hello from VM2" | sudo tee $(bao_virtio_frontend2_mp)/hello.txt > /dev/null
	@echo "Unmounting VM2..."
	sync
	sudo umount --lazy $(bao_virtio_frontend2_mp)

	# Copy to buildroot overlay
	@echo "Copying frontend images to buildroot overlay..."
	cp $(bao_virtio_frontend1_image) $(BAO_DEMOS_BUILDROOT_OVERLAY)/etc
	cp $(bao_virtio_frontend2_image) $(BAO_DEMOS_BUILDROOT_OVERLAY)/etc

	@echo "Copying scripts to buildroot overlay..."
	cp -r $(bao_virtio_setup_files)/* $(BAO_DEMOS_BUILDROOT_OVERLAY)/etc

	@echo "VirtIO demo prepared successfully!"

linux_backend_dts=$(bao_demos)/demos/$(DEMO)/devicetrees/$(PLATFORM)/linux-backend.dts
linux_backend_image=$(wrkdir_demo_imgs)/linux_backend.bin

$(linux_backend_image): prepare-virtio

$(eval $(call build-linux, $(linux_backend_image), $(linux_backend_dts)))

guest_images:=prepare-virtio $(linux_frontend1_image) $(linux_frontend2_image) $(freertos_image) $(linux_backend_image)
