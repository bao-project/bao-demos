freertos_src:=$(wrkdir_src)/freertos
freertos_repo:=https://github.com/bao-project/freertos-over-bao.git

ifeq ($(DEMO),virtio)
freertos_branch:=demo-virtio
else
freertos_branch:=demo
endif

$(freertos_src):
	git clone --recursive --shallow-submodules --branch $(freertos_branch) \
		$(freertos_repo) $(freertos_src)

freertos_bin:=$(freertos_src)/build/$(PLATFORM)/freertos.bin

define build-freertos
$(strip $1): $(freertos_src)
	$(MAKE) -C $(freertos_src) PLATFORM=$(PLATFORM) $(strip $2)
	cp $(freertos_bin) $$@
endef
