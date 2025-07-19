ifeq ($(ARCH_PROFILE), armv8-r)
fvp_link:=https://developer.arm.com/-/cdn-downloads/permalink/FVPs-Architecture/FM-11.28/FVP_Base_AEMv8R_11.28_23_Linux64.tgz
fvp_cmd:=FVP_BaseR_AEMv8R
fvp_path:=AEMv8R_base_pkg/models/Linux64_GCC-9.3/
else
fvp_link:=https://developer.arm.com/-/cdn-downloads/permalink/FVPs-Architecture/FM-11.28/FVP_Base_RevC-2xAEMvA_11.28_23_Linux64.tgz
fvp_cmd:=FVP_Base_RevC-2xAEMvA
fvp_path:=Base_RevC_AEMvA_pkg/models/Linux64_GCC-9.3/
endif

ifeq ($(shell which $(fvp_cmd)),)
fvp_cmd:=$(wrkdir_bin)/$(fvp_path)/$(fvp_cmd)
$(fvp_cmd):
	curl -L $(fvp_link) | tar xz -C $(wrkdir_bin)

fvp: $(fvp_cmd)
else
# TODO: add a version check if fvp is already installed
fvp:
endif

all:
	@printf "\nTo start fvp execute \"make PLATFORM=$(PLATFORM) DEMO=$(DEMO) run\"\n\n"

.PHONY: fvp
