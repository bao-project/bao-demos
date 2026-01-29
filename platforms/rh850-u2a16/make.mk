ARCH:=rh850

instuctions:=$(bao_demos)/platforms/$(PLATFORM)/$(DEMO).md

platform: $(bao_image)
	$(call print-instructions, $(instuctions), 1, false)
	$(call print-instructions, $(instuctions), 2, true)
