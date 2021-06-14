freertos_src:=$(wrkdir_src)/freertos
freertos_repo:=https://github.com/bao-project/freertos-over-bao.git
freertos_branch:=demo

$(freertos_src):
	git clone --recursive --shallow-submodules --branch $(freertos_branch) \
		$(freertos_repo) $(freertos_src)

freertos_bin:=$(freertos_src)/build/$(PLATFORM)/freertos.bin

freertos $(freertos_bin): $(freertos_src)
	$(MAKE) -C $(freertos_src) PLATFORM=$(PLATFORM)

define build-freertos
$(strip $1): $(freertos_bin)
	cp $$< $$@
endef
