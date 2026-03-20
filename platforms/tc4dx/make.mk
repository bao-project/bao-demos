ARCH:=tricore

instructions:=$(bao_demos)/platforms/$(PLATFORM)/README.md
template:=$(bao_demos)/platforms/$(PLATFORM)/template.cmm
script:=$(bao_demos)/platforms/$(PLATFORM)/tc4_demo.cmm

baremetal_elf:=$(wrkdir_src)/baremetal/build/tc4dx/baremetal.elf
bao_elf:=$(bao_src)/bin/tc4dx/$(DEMO)/bao.elf

#the script can run without the guest elf. That might make this script
#cleaner. It is worth consdering, and adding that to the readme.

# add a way to auto run the script after a command.

script_generation: 
	@sed \
		-e 's|@BAO_ELF@|$(bao_elf)|g' \
		-e 's|@BAREMETAL_ELF@|$(baremetal_elf)|g' \
		$(template) > $(script)

platform: $(bao_image) $(flash_image) script_generation
	$(call print-instructions, $(instructions), 1, false)
	$(call print-instructions, $(instructions), 2, false)
	$(call print-instructions, $(instructions), 3, true)

