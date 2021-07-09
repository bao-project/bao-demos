baremetal_src:=$(wrkdir_src)/baremetal
baremetal_repo:=https://github.com/bao-project/bao-baremetal-guest.git 
baremetal_branch:=rocket

$(baremetal_src):
	git clone $(baremetal_repo) $@ --branch $(baremetal_branch)

baremetal_bin:=$(baremetal_src)/build/$(PLATFORM)/baremetal.bin
baremetal $(baremetal_bin): $(baremetal_src)
	$(MAKE) -C $(baremetal_src) PLATFORM=$(PLATFORM)

define build-baremetal
$(strip $1): $(baremetal_bin)
	cp $$< $$@
endef
