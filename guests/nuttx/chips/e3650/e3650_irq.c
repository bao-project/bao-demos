#include <nuttx/config.h>

#include <stdint.h>
#include <sys/types.h>
#include <assert.h>
#include <debug.h>

#include <nuttx/arch.h>
#include <nuttx/irq.h>

#include "arm_internal.h"

#include "arm_gic.h"

#define GICV3_PRIO_SHIF 3UL

#if defined(CONFIG_SMP)
#  define E3650_IRQ_NCPUS CONFIG_SMP_NCPUS
#else
#  define E3650_IRQ_NCPUS 1
#endif

#define INTSTACK_ALLOC (E3650_IRQ_NCPUS * INTSTACK_SIZE)

#if defined(CONFIG_SMP) && CONFIG_ARCH_INTERRUPTSTACK > 7
/* In the SMP configuration, we will need custom IRQ and FIQ stacks.
 * These definitions provide the aligned stack allocations.
 */

static uint64_t g_irqstack_alloc[INTSTACK_ALLOC >> 3];
static uint64_t g_fiqstack_alloc[INTSTACK_ALLOC >> 3];

/* These are arrays that point to the top of each interrupt stack */

const uintptr_t g_irqstack_top[E3650_IRQ_NCPUS] = { (uintptr_t)g_irqstack_alloc + INTSTACK_SIZE,
#if E3650_IRQ_NCPUS > 1
						   (uintptr_t)g_irqstack_alloc + (2 * INTSTACK_SIZE),
#endif
#if E3650_IRQ_NCPUS > 2
						   (uintptr_t)g_irqstack_alloc + (3 * INTSTACK_SIZE),
#endif
#if E3650_IRQ_NCPUS > 3
						   (uintptr_t)g_irqstack_alloc + (4 * INTSTACK_SIZE)
#endif
};

const uintptr_t g_fiqstack_top[E3650_IRQ_NCPUS] = { (uintptr_t)g_fiqstack_alloc + INTSTACK_SIZE,
#if E3650_IRQ_NCPUS > 1
						   (uintptr_t)g_fiqstack_alloc + 2 * INTSTACK_SIZE,
#endif
#if E3650_IRQ_NCPUS > 2
						   (uintptr_t)g_fiqstack_alloc + 3 * INTSTACK_SIZE,
#endif
#if E3650_IRQ_NCPUS > 3
						   (uintptr_t)g_fiqstack_alloc + 4 * INTSTACK_SIZE
#endif
};

#endif

/****************************************************************************
 * Name: up_get_intstackbase
 *
 * Description:
 *   Return a pointer to the "alloc" the correct interrupt stack allocation
 *   for the current CPU.
 *
 ****************************************************************************/

#if defined(CONFIG_SMP) && CONFIG_ARCH_INTERRUPTSTACK > 7
uintptr_t up_get_intstackbase(int cpu)
{
	return g_irqstack_top[cpu] - INTSTACK_SIZE;
}
#endif

/****************************************************************************
 * Name: up_init_irq
 *
 * Description:
 *   Init the IRQ specified by 'irq'
 *
 ****************************************************************************/

void up_init_irq(int irq, int irq_prio)
{
  /* Allow guest-visible SPI priority programming through the virtual GIC
   * distributor, but leave physical interrupt routing and trigger ownership
   * to Bao.
   */
#ifdef CONFIG_ARCH_IRQPRIO
	up_prioritize_irq(irq, irq_prio << GICV3_PRIO_SHIF);
#else
  (void)irq;
  (void)irq_prio;
#endif
}

/****************************************************************************
 * Name: up_irq_is_enabled
 *
 * Description:
 *   Determine if an IRQ is enabled.
 *
 ****************************************************************************/

bool up_irq_is_enabled(int irq)
{
	return arm_gic_irq_is_enabled(irq);
}

#ifdef CONFIG_SMP
int up_send_smp_sched(int cpu)
{
  up_trigger_irq(GIC_SMP_SCHED, (cpu_set_t)1 << cpu);
  return OK;
}

void up_send_smp_call(cpu_set_t cpuset)
{
  up_trigger_irq(GIC_SMP_CALL, cpuset);
}
#endif

/****************************************************************************
 * Name: up_clear_irq
 *
 * Description:
 *   Clear the pending IRQ.
 *
 ****************************************************************************/

void up_clear_irq(int irq)
{
	/* Bao owns the physical GIC pending state for guest VMs. */
	(void)irq;
}
