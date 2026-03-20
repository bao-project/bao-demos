baremetal_src:=$(wrkdir_src)/baremetal
baremetal_repo:=https://github.com/bao-project/bao-baremetal-guest.git
baremetal_version:=8336d17e225e03d9bdd656d37804003e7f9ace81

$(baremetal_src):
	git clone $(baremetal_repo) $@
	cd $@ && git checkout $(baremetal_version)

baremetal_bin:=$(baremetal_src)/build/$(PLATFORM)/baremetal.bin

define build-baremetal
$(strip $1): $(baremetal_src)
	$(MAKE) -C $(baremetal_src) PLATFORM=$(PLATFORM) $(strip $2)
	cp $(baremetal_bin) $$@
endef
