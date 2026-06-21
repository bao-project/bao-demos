ARCH:=aarch64

ifneq ($(DEMO), torizonos+freertos)

instructions:=$(bao_demos)/platforms/$(PLATFORM)/README.md
platform: $(bao_image)
	$(call print-instructions, $(instructions), 1, false)
	$(call print-instructions, $(instructions), 2, false)
	$(call print-instructions, $(instructions), 3, true)

else

guest_instructions:=$(bao_demos)/guests/torizonos/README.md
plat_instructions:=$(bao_demos)/platforms/$(PLATFORM)/TORIZONOS-README.md
platform: $(bao_image)
	$(call print-instructions, $(guest_instructions), 1, false)
	$(call print-instructions, $(guest_instructions), 2, false)
	$(call print-instructions, $(guest_instructions), 3, false)
	$(call print-instructions, $(guest_instructions), 4, false)
	$(call print-instructions, $(plat_instructions), 1, false)
	$(call print-instructions, $(plat_instructions), 2, true)
endif
