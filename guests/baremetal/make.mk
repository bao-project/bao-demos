baremetal_src:=$(wrkdir_src)/baremetal
baremetal_repo:=https://github.com/bao-project/bao-baremetal-guest.git
baremetal_version:=7c3a0f3c9f81e8a2fe34af6db1e39efb81c166af

$(baremetal_src):
	git clone $(baremetal_repo) $@
	cd $@ && git checkout $(baremetal_version)

baremetal_bin:=$(baremetal_src)/build/$(PLATFORM)/baremetal.bin

define build-baremetal
$(strip $1): $(baremetal_src)
	$(MAKE) -C $(baremetal_src) PLATFORM=$(PLATFORM) $(strip $2)
	cp $(baremetal_bin) $$@
endef
