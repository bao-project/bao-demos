#ifndef __CHIPS_E3650_CHIP_ASM_H
#define __CHIPS_E3650_CHIP_ASM_H

 #include <nuttx/config.h>

#ifdef __ASSEMBLY__

/****************************************************************************
 * Name: cpuindex
 *
 * Description:
 *   Return an index idenifying the current CPU.
 *
 ****************************************************************************/

#if defined(CONFIG_SMP) && CONFIG_ARCH_INTERRUPTSTACK > 7
  .macro  cpuindex, index
  mrc  p15, 0, \index, c0, c0, 5  /* Read the MPIDR */
  and  \index, \index, #3         /* Bits 0-1=CPU ID */
  .endm
#endif

/****************************************************************************
 * Name: setirqstack
 *
 * Description:
 *   Set the current stack pointer to the  -"top" of the IRQ interrupt
 *   stack for the current CPU.
 *
 ****************************************************************************/

#if defined(CONFIG_SMP) && CONFIG_ARCH_INTERRUPTSTACK > 7
  .macro  setirqstack, tmp1, tmp2
  mrc  p15, 0, \tmp1, c0, c0, 5  /* tmp1=MPIDR */
  and  \tmp1, \tmp1, #3          /* Bits 0-1=CPU ID */
  ldr  \tmp2, =g_irqstack_top    /* tmp2=Array of IRQ stack pointers */
  lsls \tmp1, \tmp1, #2          /* tmp1=Array byte offset */
  add  \tmp2, \tmp2, \tmp1       /* tmp2=Offset address into array */
  ldr  sp, [\tmp2, #0]           /* sp=Address in stack allocation */
  .endm
#endif

/****************************************************************************
 * Name: setfiqstack
 *
 * Description:
 *   Set the current stack pointer to the  -"top" of the FIQ interrupt
 *   stack for the current CPU.
 *
 ****************************************************************************/

#if defined(CONFIG_SMP) && CONFIG_ARCH_INTERRUPTSTACK > 7
  .macro  setfiqstack, tmp1, tmp2
  mrc  p15, 0, \tmp1, c0, c0, 5  /* tmp1=MPIDR */
  and  \tmp1, \tmp1, #3          /* Bits 0-1=CPU ID */
  ldr  \tmp2, =g_fiqstack_top    /* tmp2=Array of FIQ stack pointers */
  lsls \tmp1, \tmp1, #2          /* tmp1=Array byte offset */
  add  \tmp2, \tmp2, \tmp1       /* tmp2=Offset address into array */
  ldr  sp, [\tmp2, #0]           /* sp=Address in stack allocation */
  .endm
#endif

#endif /* __ASSEMBLY__ */

#endif /* __CHIPS_E3650_CHIP_ASM_H */
