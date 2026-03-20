BAO_DRIVERS_SITE = https://github.com/bao-project/bao-linux-drivers.git
ifneq (,$(filter $(PLATFORM),s32g3))
BAO_DRIVERS_VERSION = linux-v6.6
else
BAO_DRIVERS_VERSION = linux-v6.15
endif

BAO_DRIVERS_SITE_METHOD = git

BAO_DRIVERS_MODULE_SUBDIRS = ipc iodispatcher

$(eval $(kernel-module))
$(eval $(generic-package))
