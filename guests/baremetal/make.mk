baremetal_src:=$(wrkdir_src)/baremetal
baremetal_repo:=https://github.com/bao-project/bao-baremetal-guest.git
baremetal_version:=5821efb7ae0cf5718aa8dcc6d735019eea212cb8

$(baremetal_src):
	git clone $(baremetal_repo) $@
	cd $@ && git checkout $(baremetal_version)

baremetal_bin:=$(baremetal_src)/build/$(PLATFORM)/baremetal.bin

define build-baremetal
$(strip $1): $(baremetal_src)
	$(MAKE) -C $(baremetal_src) PLATFORM=$(PLATFORM) $(strip $2)
	cp $(baremetal_bin) $$@
endef
