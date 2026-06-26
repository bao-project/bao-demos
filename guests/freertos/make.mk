freertos_src:=$(wrkdir_src)/freertos
freertos_repo:=https://github.com/bao-project/freertos-over-bao.git
freertos_branch:=a4787e224ac20ab2fc4601d1a7f6bbe7ada84b9b

$(freertos_src):
	git clone --recursive --shallow-submodules  \
		$(freertos_repo) $(freertos_src) && \
	git checkout d4730617221e9f5f5f344fa8b1052b9c6aeaa217

freertos_bin:=$(freertos_src)/build/$(PLATFORM)/freertos.bin

define build-freertos
$(strip $1): $(freertos_src)
	$(MAKE) -C $(freertos_src) PLATFORM=$(PLATFORM) $(strip $2)
	cp $(freertos_bin) $$@
endef
