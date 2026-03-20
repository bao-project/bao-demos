################################################################################
# bao-virtio-dm Buildroot package
################################################################################

BAO_VIRTIO_DM_VERSION = main
BAO_VIRTIO_DM_SITE = https://github.com/bao-project/bao-virtio-dm.git
BAO_VIRTIO_DM_SITE_METHOD = git

BAO_VIRTIO_DM_LICENSE = Apache-2.0
BAO_VIRTIO_DM_LICENSE_FILES = LICENSE

BAO_VIRTIO_DM_TARGET = $(RUSTC_TARGET_NAME)
BAO_VIRTIO_DM_DEPENDENCIES = host-rustc

BAO_VIRTIO_DM_CARGO_ENV = \
			  CARGO_HOME=$(HOST_DIR)/share/cargo \
			  RUST_TARGET_PATH=$(HOST_DIR)/etc/rustc \
			  CARGO_TARGET_$(call UPPERCASE,$(subst -,_,$(BAO_VIRTIO_DM_TARGET)))_LINKER=$(TARGET_CC)

BAO_VIRTIO_DM_BIN_DIR = target/$(BAO_VIRTIO_DM_TARGET)/release

BAO_VIRTIO_DM_CARGO_OPTS = \
			   --release \
			   --locked \
			   --target=$(BAO_VIRTIO_DM_TARGET)
define BAO_VIRTIO_DM_BUILD_CMDS
	cd $(@D) && \
	$(TARGET_MAKE_ENV) $(BAO_VIRTIO_DM_CARGO_ENV) \
		cargo build $(BAO_VIRTIO_DM_CARGO_OPTS)
endef

define BAO_VIRTIO_DM_INSTALL_TARGET_CMDS
$(INSTALL) -D -m 0755 \
	$(@D)/$(BAO_VIRTIO_DM_BIN_DIR)/bao-virtio-dm \
	$(TARGET_DIR)/bin/bao-virtio-dm
endef

define BAO_VIRTIO_DM_CLEAN_CMDS
cd $(@D) && cargo clean
endef

$(eval $(generic-package))
