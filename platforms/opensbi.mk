opensbi_repo:=https://github.com/bao-project/opensbi.git
opensbi_version:=bao/demo-next
opensbi_src:=$(wrkdir_src)/opensbi

ifeq ($(OPENSBI_CROSS_COMPILE),)
$(error 'OPENSBI_CROSS_COMPILE' not set)
endif

$(opensbi_src):
	git clone --depth 1 --branch $(opensbi_version) $(opensbi_repo) $(opensbi_src)

define build-opensbi-payload
$(strip $1): $(strip $2) $(opensbi_src)
	$(MAKE) -C $(opensbi_src) \
		CROSS_COMPILE=$(OPENSBI_CROSS_COMPILE) \
		PLATFORM=generic \
		PLATFORM_RISCV_XLEN=$(strip $3) \
		FW_PAYLOAD=y \
		FW_PAYLOAD_FDT_ADDR=0x80100000\
		FW_PAYLOAD_PATH=$(strip $2)
	cp $(opensbi_src)/build/platform/generic/firmware/fw_payload.bin $$@
endef
