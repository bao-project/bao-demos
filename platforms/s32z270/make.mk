ARCH:=aarch32
ARCH_PROFILE:=armv8-r

instructions:=$(bao_demos)/platforms/$(PLATFORM)/README.md

platform: $(bao_image)
	$(call print-instructions, $(instructions), 1, false)
	$(call print-instructions, $(instructions), 2, false)
	$(call print-instructions, $(instructions), 3, true)
