SHELL:=bash

bao_demos:=$(abspath .)
platform_dir:=$(bao_demos)/platforms/$(PLATFORM)
demo_dir:=$(bao_demos)/demos/$(DEMO)
SHELL:=/bin/bash

ifeq ($(filter clean distclean, $(MAKECMDGOALS)),)
ifndef CROSS_COMPILE
 $(error No CROSS_COMPILE prefix defined)
endif
endif

ifeq ($(filter distclean, $(MAKECMDGOALS)),)

ifndef PLATFORM
 $(error No target PLATFORM defined)
endif

ifeq ($(wildcard $(platform_dir)),)
 $(error Target platform $(PLATFORM) is not supported)
endif

ifndef DEMO
 $(error No target DEMO defined.)
endif
    
ifeq ($(wildcard $(demo_dir)),)
 $(error Target demo $(DEMO) is not supported)
endif

ifeq ($(wildcard $(demo_dir)/configs/$(PLATFORM).c),)
 $(error The $(DEMO) demo is not supported by the $(PLATFORM) platform)
endif

endif 

# utility functions

ifeq ($(NO_INSTRUCTIONS),)
define print-instructions
	@for i in {1..80}; do printf "-"; done ; printf "\n"
	@cat $(strip $1) | sed -n '/instruction#$(strip $2)/,/instruction#./p' |\
		sed '1d;$d' | head -n -1 |\
		sed -r -e 's/(.*)\[(.*)\]\((http.*)\)(.*)/\1\2 (\3)\4/g' |\
		sed -r -e 's/(.*)\[(.*)\]\((\.\/(\.\.\/)*)(.*)\)(.*)/\1\2 (\.\/\5)\6/g' |\
		pandoc --to plain --wrap=preserve | $(environment) envsubst
	-@if [ $(strip $3) = false ];\
		then  printf "\n(Press return to continue)\r"; read -s dummy;\
		else for i in {1..80}; do printf "-"; done ; printf "\n"; fi
endef
endif

# setup working directories

wrkdir:=$(bao_demos)/wrkdir
wrkdir_src:=$(wrkdir)/srcs
wrkdir_bin:=$(wrkdir)/bin
wrkdir_imgs:=$(wrkdir)/imgs
wrkdir_plat_imgs:=$(wrkdir_imgs)/$(PLATFORM)
wrkdir_demo_imgs:=$(wrkdir_plat_imgs)/$(DEMO)
wrkdirs=$(wrkdir) $(wrkdir_src) $(wrkdir_bin) $(wrkdir_plat_imgs) $(wrkdir_demo_imgs)

environment:=BAO_DEMOS=$(bao_demos)
environment+=BAO_DEMOS_WRKDIR=$(wrkdir)
environment+=BAO_DEMOS_WRKDIR_SRC=$(wrkdir_src)
environment+=BAO_DEMOS_WRKDIR_PLAT=$(wrkdir_plat_imgs)
environment+=BAO_DEMOS_WRKDIR_IMGS=$(wrkdir_demo_imgs)
environment+=BAO_DEMOS_SDCARD_DEV=/dev/yoursdcarddev
environment+=BAO_DEMOS_SDCARD=/media/$$USER/boot

all: platform 

bao_repo:=https://github.com/bao-project/bao-hypervisor
bao_version:=main
bao_src:=$(wrkdir_src)/bao
bao_cfg_repo:=$(wrkdir_demo_imgs)/config
wrkdirs+=$(bao_cfg_repo)
bao_cfg:=$(bao_cfg_repo)/$(DEMO).c
bao_image:=$(wrkdir_demo_imgs)/bao.bin

include $(platform_dir)/make.mk
include $(demo_dir)/make.mk

ifeq ($(filter clean distclean, $(MAKECMDGOALS)),)
$(shell mkdir -p $(wrkdirs))
endif

guests: $(guest_images)

$(bao_src):
	git clone --branch $(bao_version) $(bao_repo) $(bao_src)

$(bao_cfg): | $(bao_cfg_repo)
	cp -L $(bao_demos)/demos/$(DEMO)/configs/$(PLATFORM).c $(bao_cfg)

bao $(bao_image): $(guest_images) $(bao_cfg) $(bao_src) 
	$(MAKE) -C $(bao_src)\
		PLATFORM=$(PLATFORM)\
		CONFIG_REPO=$(bao_cfg_repo)\
		CONFIG=$(DEMO) \
		CPPFLAGS=-DBAO_DEMOS_WRKDIR_IMGS=$(wrkdir_demo_imgs)
	cp $(bao_src)/bin/$(PLATFORM)/$(DEMO)/bao.bin $(bao_image)

bao_clean:
	$(MAKE) -C $(bao_src) clean\
		PLATFORM=$(PLATFORM)\
		CONFIG_REPO=$(bao_cfg_repo)\
		CONFIG=$(DEMO)\
		CPPFLAGS=-DBAO_DEMOS_WRKDIR_IMGS=$(wrkdir_demo_imgs)

platform: $(bao_image)

guests_clean bao_clean platform_clean:

clean: guests_clean bao_clean platform_clean
	-@rm -rf $(wrkdir)/imgs/$(PLATFORM)/$(DEMO)

distclean:
	rm -rf $(wrkdir)

.PHONY: all clean guests bao platform
.NOTPARALLEL:
