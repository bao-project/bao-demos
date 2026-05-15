freertos_src:=$(wrkdir_src)/freertos
freertos_repo:=https://github.com/bao-project/freertos-over-bao.git
freertos_version:=a4787e224ac20ab2fc4601d1a7f6bbe7ada84b9b

$(freertos_src):
	git clone $(freertos_repo) $@
	cd $@ && \
		git checkout $(freertos_version) && \
		git submodule update --init --recursive

freertos_bin:=$(freertos_src)/build/$(PLATFORM)/freertos.bin

define build-freertos
$(strip $1): $(freertos_src)
	$(MAKE) -C $(freertos_src) PLATFORM=$(PLATFORM) $(strip $2)
	cp $(freertos_bin) $$@
endef
