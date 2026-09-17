ARCH:=tricore

instructions:=$(bao_demos)/platforms/$(PLATFORM)/README.md
template:=$(bao_demos)/platforms/$(PLATFORM)/template.cmm
script:=$(bao_demos)/platforms/$(PLATFORM)/tc4_demo.cmm

baremetal_elf:=$(wrkdir_src)/baremetal/build/tc4dx/baremetal.elf
zephyr_elf:=$(wrkdir_demo_imgs)/zephyr_build/zephyr/zephyr.elf
bao_elf:=$(bao_src)/bin/tc4dx/$(DEMO)/bao.elf

# Guest ELF substitutions are for TRACE32 debug symbols only (/NoCode
# /NoClear in the template); the binaries themselves are already embedded in
# bao.elf via VM_IMAGE(). A demo missing one guest (e.g. single-guest
# `baremetal` has no zephyr_elf) is fine -- the template's OS.FILE() guard
# skips a nonexistent path instead of erroring.
#
# add a way to auto run the script after a command.

script_generation:
	@sed \
		-e 's|@BAO_ELF@|$(bao_elf)|g' \
		-e 's|@BAREMETAL_ELF@|$(baremetal_elf)|g' \
		-e 's|@ZEPHYR_ELF@|$(zephyr_elf)|g' \
		$(template) > $(script)

platform: $(bao_image) $(flash_image) script_generation
	$(call print-instructions, $(instructions), 1, false)
	$(call print-instructions, $(instructions), 2, false)
	$(call print-instructions, $(instructions), 3, true)

