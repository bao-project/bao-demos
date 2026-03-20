/* Simplified linker script for NuttX running as BAO guest on E3650 */

#include <nuttx/config.h>

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(__start)

#define HEAP_SIZE 0x10000
#ifdef CONFIG_SMP
#  define PER_CPU_RESERVED_SIZE 0x3000
#endif

/* BAO VM memory allocation: 0x00BA0000 - 0x00CB0000 (1.0625 MB) */
MEMORY
{
    RAM (rwx) : ORIGIN = 0x00BA0000, LENGTH = 0x00110000
}

SECTIONS
{
    .text : ALIGN(64) {
        . = ALIGN(64);
        KEEP(*(.sys_vectors))
        . = ALIGN(64);
        _stext = ABSOLUTE(.);
        KEEP(*(.hyp_vectors))
        *(.text .text.*)
        *(.fixup)
        *(.gnu.warning)
        *(.gnu.linkonce.t.*)
        *(.glue_7)
        *(.glue_7t)
        *(.got)
        *(.gcc_except_table)
        *(.gnu.linkonce.r.*)
        *(.ARM.extab*)
        *(.gnu.linkonce.armextab.*)
        . = ALIGN(64);
        _etext = ABSOLUTE(.);
    } > RAM

    .init_section : {
        . = ALIGN(64);
        _sinit = ABSOLUTE(.);
        *(SORT(.ctors.*))
        SORT(CONSTRUCTORS)
        KEEP(*(.preinit_array))
        KEEP(*(.init_array .init_array.*))
        _einit = ABSOLUTE(.);
    } > RAM

    .rodata : ALIGN(4) {
        _srodata = .;
        *(.rodata .rodata.*)
        _erodata = .;
    } > RAM

    _eronly = LOADADDR(.data);
    _load_data_start = _eronly;

    .data : ALIGN(64) {
        _sdata = .;
        _data_start = .;
        *(.data .data.*)
        . = ALIGN(64);
        _edata = .;
        _data_end = .;
    } > RAM

    .bss (NOLOAD) : ALIGN(64) {
        _sbss = .;
        *(.bss .bss.*)
        *(.gnu.linkonce.b.*)
        *(COMMON)
        . = ALIGN(64);
        _ebss = .;
    } > RAM

    /* DMA buffer symbols (not used but required by code) */
    _dma_buffer_start = .;
    _dma_buffer_end = .;

#ifdef CONFIG_SMP
    .core0_percpu_data : ALIGN(64) {
        __PERCPU0_START = .;
        _core0_percpu_data_start = .;
        *(.data..percpu*)
        . = ALIGN(64);
        _core0_percpu_data_end = .;
    } > RAM

    .core0_percpu_bss (NOLOAD) : ALIGN(64) {
        _core0_percpu_bss_start = .;
        *(.bss..percpu*)
        . = ALIGN(64);
        _core0_percpu_bss_end = .;
        __PERCPU0_END = . - 1;
    } > RAM

#  if (CONFIG_SMP_NCPUS > 1)
    .core1_percpu_reserve (NOLOAD) : ALIGN(64) {
        __PERCPU1_START = .;
        _core1_percpu_reserve_start = .;
        . += PER_CPU_RESERVED_SIZE;
        __PERCPU1_END = . - 1;
    } > RAM
#  endif
#  if (CONFIG_SMP_NCPUS > 2)
    .core2_percpu_reserve (NOLOAD) : ALIGN(64) {
        __PERCPU2_START = .;
        _core2_percpu_reserve_start = .;
        . += PER_CPU_RESERVED_SIZE;
        __PERCPU2_END = . - 1;
    } > RAM
#  endif
#  if (CONFIG_SMP_NCPUS > 3)
    .core3_percpu_reserve (NOLOAD) : ALIGN(64) {
        __PERCPU3_START = .;
        _core3_percpu_reserve_start = .;
        . += PER_CPU_RESERVED_SIZE;
        __PERCPU3_END = . - 1;
    } > RAM
#  endif
#endif

    /* Idle stack for core 0 */
    .idle_stack (NOLOAD) : ALIGN(64) {
        _core0_idle_stack_start = .;
        . += 0x2000;  /* 8KB idle stack */
        _core0_idle_stack_end = .;
#if defined(CONFIG_SMP) && (CONFIG_SMP_NCPUS > 1)
        _core1_idle_stack_start = .;
        . += 0x2000;
        _core1_idle_stack_end = .;
#endif
#if defined(CONFIG_SMP) && (CONFIG_SMP_NCPUS > 2)
        _core2_idle_stack_start = .;
        . += 0x2000;
        _core2_idle_stack_end = .;
#endif
#if defined(CONFIG_SMP) && (CONFIG_SMP_NCPUS > 3)
        _core3_idle_stack_start = .;
        . += 0x2000;
        _core3_idle_stack_end = .;
#endif
    } > RAM

    /* Per-core heaps for BAO guest - all allocated from guest RAM */
    .heap (NOLOAD) : ALIGN(64) {
        __HEAP0_START = .;
        _core0_heap_start = .;
        . += HEAP_SIZE;
        _core0_heap_end = .;
        __HEAP0_END = . - 1;
#if defined(CONFIG_SMP) && (CONFIG_SMP_NCPUS > 1)
        __HEAP1_START = .;
        _core1_heap_start = .;
        . += HEAP_SIZE;
        _core1_heap_end = .;
        __HEAP1_END = . - 1;
#endif
#if defined(CONFIG_SMP) && (CONFIG_SMP_NCPUS > 2)
        __HEAP2_START = .;
        _core2_heap_start = .;
        . += HEAP_SIZE;
        _core2_heap_end = .;
        __HEAP2_END = . - 1;
#endif
#if defined(CONFIG_SMP) && (CONFIG_SMP_NCPUS > 3)
        __HEAP3_START = .;
        _core3_heap_start = .;
        . += HEAP_SIZE;
        _core3_heap_end = .;
        __HEAP3_END = . - 1;
#endif
    } > RAM

    /* Dummy symbols for unused additional cores */
#if !defined(CONFIG_SMP) || (CONFIG_SMP_NCPUS <= 1)
    _core1_heap_start = _core0_heap_end;
    _core1_heap_end = _core0_heap_end;
#endif
#if !defined(CONFIG_SMP) || (CONFIG_SMP_NCPUS <= 2)
    _core2_heap_start = _core0_heap_end;
    _core2_heap_end = _core0_heap_end;
#endif
#if !defined(CONFIG_SMP) || (CONFIG_SMP_NCPUS <= 3)
    _core3_heap_start = _core0_heap_end;
    _core3_heap_end = _core0_heap_end;
#endif

#if !defined(CONFIG_SMP) || (CONFIG_SMP_NCPUS <= 1)
    _core1_percpu_reserve_start = _core0_heap_end;
#endif
#if !defined(CONFIG_SMP) || (CONFIG_SMP_NCPUS <= 2)
    _core2_percpu_reserve_start = _core0_heap_end;
#endif
#if !defined(CONFIG_SMP) || (CONFIG_SMP_NCPUS <= 3)
    _core3_percpu_reserve_start = _core0_heap_end;
#endif

    _enoinit = .;

    /DISCARD/ : {
        *(.comment .note .eh_frame)
    }
}
