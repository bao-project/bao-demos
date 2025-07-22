BAO_DRIVERS_SITE = https://github.com/bao-project/bao-linux-drivers.git
BAO_DRIVERS_VERSION = linux-v6.15
BAO_DRIVERS_SITE_METHOD = git

BAO_DRIVERS_MODULE_SUBDIRS = ipc

$(eval $(kernel-module))
$(eval $(generic-package))
