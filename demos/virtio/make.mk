include $(bao_demos)/guests/linux/make.mk
include $(bao_demos)/guests/freertos/make.mk

linux_frontend1_image=$(wrkdir_demo_imgs)/linux_frontend1.bin
linux_frontend2_image=$(wrkdir_demo_imgs)/linux_frontend2.bin

linux_frontend1_dts=$(bao_demos)/demos/$(DEMO)/devicetrees/$(PLATFORM)/linux-frontend1.dts
linux_frontend2_dts=$(bao_demos)/demos/$(DEMO)/devicetrees/$(PLATFORM)/linux-frontend2.dts

$(eval $(call build-linux, $(linux_frontend1_image), $(linux_frontend1_dts)))
$(eval $(call build-linux, $(linux_frontend2_image), $(linux_frontend2_dts)))

freertos_image:=$(wrkdir_demo_imgs)/freertos.bin

make_args:=STD_ADDR_SPACE=y

$(eval $(call build-freertos, $(freertos_image), $(make_args)))

export BAO_DEMOS_BUILDROOT_POST_BUILD_SCRIPT=$(bao_demos)/demos/$(DEMO)/setup/post-build.sh

# bao_iodispatcher_image:=$(wrkdir_demo_imgs)/iodispatcher.ko
# $(eval $(call build-linux-drivers, $(bao_iodispatcher_image)))

bao_virtio_dm_src:=$(wrkdir_src)/bao-virtio-dm
bao_virtio_dm_repo:=https://github.com/bao-project/bao-virtio-dm.git
bao_virtio_dm_image:=$(wrkdir_demo_imgs)/bao-virtio-dm
bao_virtio_setup_files:=$(bao_demos)/demos/$(DEMO)/setup
bao_virtio_frontend1_image:=$(wrkdir_demo_imgs)/ext4_frontend_vm1_image.img
bao_virtio_frontend2_image:=$(wrkdir_demo_imgs)/ext4_frontend_vm2_image.img
bao_virtio_frontend1_mp:=$(wrkdir_src)/ext4_frontend_vm1
bao_virtio_frontend2_mp:=$(wrkdir_src)/ext4_frontend_vm2
pwd:=$(shell pwd)

ifeq ($(ARCH),aarch64)
bao_virtio_dm_target:=aarch64-unknown-linux-gnu
else ifeq ($(ARCH),riscv64)
bao_virtio_dm_target:=riscv64gc-unknown-linux-gnu
else
bao_virtio_dm_target:=arm-unknown-linux-gnueabi
endif

$(bao_virtio_dm_src):
	git clone --depth 1 --branch main $(bao_virtio_dm_repo) $(bao_virtio_dm_src)

define build-bao-virtio-dm
$(strip $1): $(bao_virtio_dm_src)
	cd $(bao_virtio_dm_src) && cargo build --target $(bao_virtio_dm_target) --release
	cp $(bao_virtio_dm_src)/target/$(bao_virtio_dm_target)/release/bao-virtio-dm $$@ && cd $(pwd)
	cp $(bao_virtio_dm_src)/target/$(bao_virtio_dm_target)/release/bao-virtio-dm $(BAO_DEMOS_BUILDROOT_OVERLAY_DIR)/bin
	cp -r $(bao_virtio_setup_files)/* $(BAO_DEMOS_BUILDROOT_OVERLAY_DIR)/etc

	# Print message for sudo operations
	echo "This operation requires sudo privileges to perform tasks that involve mounting the Linux frontend disk images and formatting them."
	echo "Specifically, we will: "
	echo " 1. Create two disk images of 1MB each for the Linux frontends."
	echo " 2. Format the two disk images as ext4 file systems."
	echo " 3. Mount these disk images in two directories to add some test data."
	echo " 4. Unmount the disk images after the operations are completed."
	echo "Please ensure you have sudo access and the necessary permissions to perform these tasks."

	# Create disk images
	dd if=/dev/zero of=$(bao_virtio_frontend1_image) bs=1M count=1
	dd if=/dev/zero of=$(bao_virtio_frontend2_image) bs=1M count=1
	
	# Use sudo for operations that need elevated privileges
	echo "Formatting VM1 disk image to ext4..."
	sudo mkfs.ext4 $(bao_virtio_frontend1_image)
	
	echo "Formatting VM2 disk image to ext4..."
	sudo mkfs.ext4 $(bao_virtio_frontend2_image)
	
	echo "Mounting VM1 disk image..."
	mkdir -p $(bao_virtio_frontend1_mp)
	sudo mount -o loop $(bao_virtio_frontend1_image) $(bao_virtio_frontend1_mp)
	# Add some test data
	echo "Hello from VM1" | sudo tee $(bao_virtio_frontend1_mp)/hello.txt > /dev/null
	# Unmount after finishing operations
	echo "Unmounting VM1 disk image..."
	sync
	sudo umount --lazy $(bao_virtio_frontend1_mp)
	
	echo "Mounting VM2 disk image..."
	mkdir -p $(bao_virtio_frontend2_mp)
	sudo mount -o loop $(bao_virtio_frontend2_image) $(bao_virtio_frontend2_mp)
	# Add some test data
	echo "Hello from VM2" | sudo tee $(bao_virtio_frontend2_mp)/hello.txt > /dev/null
	# Unmount after finishing operations
	echo "Unmounting VM2 disk image..."
	sync
	sudo umount --lazy $(bao_virtio_frontend2_mp)

	# Copy the disk images to the buildroot overlay directory
	cp $(bao_virtio_frontend1_image) $(BAO_DEMOS_BUILDROOT_OVERLAY_DIR)/etc
	cp $(bao_virtio_frontend2_image) $(BAO_DEMOS_BUILDROOT_OVERLAY_DIR)/etc
endef

$(eval $(call build-bao-virtio-dm, $(bao_virtio_dm_image)))

bao_iodispatcher_image:=$(wrkdir_demo_imgs)/iodispatcher.ko
$(eval $(call build-linux-drivers, $(bao_iodispatcher_image)))

linux_backend_dts=$(bao_demos)/demos/$(DEMO)/devicetrees/$(PLATFORM)/linux-backend.dts
linux_backend_image=$(wrkdir_demo_imgs)/linux_backend.bin

$(eval $(call build-linux, $(linux_backend_image), $(linux_backend_dts)))

guest_images:=$(linux_frontend1_image) $(linux_frontend2_image) $(freertos_image) $(bao_virtio_dm_image) $(bao_iodispatcher_image) $(linux_backend_image)

run:
	$(call print-instructions, $(bao_demos)/demos/virtio/README.md, 1, false)
	$(call print-instructions, $(bao_demos)/demos/virtio/README.md, 2, false)
	$(call print-instructions, $(bao_demos)/demos/virtio/README.md, 3, false)
	$(call print-instructions, $(bao_demos)/demos/virtio/README.md, 4, false)
	$(call print-instructions, $(bao_demos)/demos/virtio/README.md, 5, true)