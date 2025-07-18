baremetal_src:=$(wrkdir_src)/baremetal
baremetal_repo:=https://github.com/bao-project/bao-baremetal-guest.git 
baremetal_branch:=demo-next

$(baremetal_src):
	git clone $(baremetal_repo) $@ --branch $(baremetal_branch)

baremetal_bin:=$(baremetal_src)/build/$(PLATFORM)/baremetal.bin

define build-baremetal
$(strip $1): $(baremetal_src)
	$(MAKE) -C $(baremetal_src) PLATFORM=$(PLATFORM) $(strip $2) 
	cp $(baremetal_bin) $$@
endef
