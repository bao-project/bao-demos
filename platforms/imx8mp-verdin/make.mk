ARCH:=aarch64

instructions:=$(bao_demos)/platforms/$(PLATFORM)/README.md
platform: $(bao_image)
	$(call print-instructions, $(instructions), 1, false)
	$(call print-instructions, $(instructions), 2, false)
	$(call print-instructions, $(instructions), 3, true)
