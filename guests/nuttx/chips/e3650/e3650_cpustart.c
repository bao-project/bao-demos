#include <nuttx/config.h>

#ifdef CONFIG_SMP

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <debug.h>

#include <nuttx/arch.h>
#include <nuttx/irq.h>
#include <nuttx/sched.h>

#include "init/init.h"

extern uint8_t _core1_idle_stack_start[];
extern uint8_t _core1_idle_stack_end[];
#if CONFIG_SMP_NCPUS > 2
extern uint8_t _core2_idle_stack_start[];
extern uint8_t _core2_idle_stack_end[];
#endif
#if CONFIG_SMP_NCPUS > 3
extern uint8_t _core3_idle_stack_start[];
extern uint8_t _core3_idle_stack_end[];
#endif
extern void __cpu1_start(void);
#if CONFIG_SMP_NCPUS > 2
extern void __cpu2_start(void);
#endif
#if CONFIG_SMP_NCPUS > 3
extern void __cpu3_start(void);
#endif
void arm_timer_secondary_init(unsigned int freq);
void up_putc(int ch);

#define PSCI_CPU_ON_AARCH32   (0x84000003UL)
#define PSCI_E_SUCCESS        (0)
#define PSCI_E_ALREADY_ON     (-4)
#define E3650_GUEST_MAX_CPUS  (4)

static const unsigned long g_e3650_guest_mpidr[E3650_GUEST_MAX_CPUS] =
{
  0x80000000UL,
  0x80000001UL,
  0x80000002UL,
  0x80000003UL,
};

static uint8_t * const g_e3650_idle_stack_start[E3650_GUEST_MAX_CPUS] =
{
  NULL,
  _core1_idle_stack_start,
#if CONFIG_SMP_NCPUS > 2
  _core2_idle_stack_start,
#else
  NULL,
#endif
#if CONFIG_SMP_NCPUS > 3
  _core3_idle_stack_start,
#else
  NULL,
#endif
};

static uint8_t * const g_e3650_idle_stack_end[E3650_GUEST_MAX_CPUS] =
{
  NULL,
  _core1_idle_stack_end,
#if CONFIG_SMP_NCPUS > 2
  _core2_idle_stack_end,
#else
  NULL,
#endif
#if CONFIG_SMP_NCPUS > 3
  _core3_idle_stack_end,
#else
  NULL,
#endif
};

static void (* const g_e3650_secondary_entry[E3650_GUEST_MAX_CPUS])(void) =
{
  NULL,
  __cpu1_start,
#if CONFIG_SMP_NCPUS > 2
  __cpu2_start,
#else
  NULL,
#endif
#if CONFIG_SMP_NCPUS > 3
  __cpu3_start,
#else
  NULL,
#endif
};

static void e3650_cpu_puts(const char *str)
{
  while (*str != '\0')
    {
      up_putc(*str++);
    }
}

static int e3650_hvc_call(unsigned long x0, unsigned long x1,
                          unsigned long x2, unsigned long x3)
{
  register unsigned long r0 asm("r0") = x0;
  register unsigned long r1 asm("r1") = x1;
  register unsigned long r2 asm("r2") = x2;
  register unsigned long r3 asm("r3") = x3;

  asm volatile(
      "hvc #0\n"
      : "=r"(r0)
      : "r"(r0), "r"(r1), "r"(r2), "r"(r3)
      : "memory");

  return (int)r0;
}

int up_cpu_idlestack(int cpu, struct tcb_s *tcb, size_t stack_size)
{
  DEBUGASSERT(cpu > 0 && cpu < CONFIG_SMP_NCPUS);
  DEBUGASSERT(tcb != NULL);
  DEBUGASSERT(cpu < E3650_GUEST_MAX_CPUS);
  DEBUGASSERT(g_e3650_idle_stack_start[cpu] != NULL);
  DEBUGASSERT(g_e3650_idle_stack_end[cpu] != NULL);
  DEBUGASSERT(stack_size <=
              (size_t)(g_e3650_idle_stack_end[cpu] -
                       g_e3650_idle_stack_start[cpu]));

  tcb->adj_stack_size  = (size_t)(g_e3650_idle_stack_end[cpu] -
                                  g_e3650_idle_stack_start[cpu]);
  tcb->stack_alloc_ptr = (void *)g_e3650_idle_stack_start[cpu];
  tcb->stack_base_ptr  = tcb->stack_alloc_ptr;
  return OK;
}

int up_cpu_start(int cpu)
{
  int ret;
  unsigned long target_cpu;
  void (*entry)(void);

  if (cpu <= 0 || cpu >= CONFIG_SMP_NCPUS)
    {
      return -EINVAL;
    }

  if (cpu >= E3650_GUEST_MAX_CPUS)
    {
      return -ENOTSUP;
    }

  entry = g_e3650_secondary_entry[cpu];
  if (entry == NULL)
    {
      return -ENOTSUP;
    }

  /* Under Bao, secondary vCPUs start in PSCI OFF state. Request the selected
   * CPU to start at its dedicated secondary entrypoint and keep SEV as a
   * fallback nudge for any path that might still be waiting in WFE.
   */

  target_cpu = g_e3650_guest_mpidr[cpu];

  ret = e3650_hvc_call(PSCI_CPU_ON_AARCH32, target_cpu,
                       (unsigned long)entry, 0);

  if (ret != PSCI_E_SUCCESS && ret != PSCI_E_ALREADY_ON)
    {
      return -ret;
    }

  __asm__ volatile ("dsb sy\n"
                    "sev\n"
                    "isb\n"
                    :
                    :
                    : "memory");

  return OK;
}

void arm_cpu_boot(int cpu)
{
  DEBUGASSERT(cpu > 0 && cpu < CONFIG_SMP_NCPUS);

  /* Initialize per-CPU interrupt state before entering the secondary idle
   * trampoline.
   */

  up_irqinitialize();
  arm_timer_secondary_init(0);
  e3650_cpu_puts("NX cpu ");
  up_putc('0' + cpu);
  e3650_cpu_puts(" up\n");
  nx_idle_trampoline();
}

#endif /* CONFIG_SMP */
