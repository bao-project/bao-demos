freertos_src:=$(wrkdir_src)/freertos
freertos_repo:=https://github.com/bao-project/freertos-over-bao.git
freertos_version:=cb9112f982c2768872536b811e013254d0184811

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
