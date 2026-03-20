#ifndef __CHIPS_E3650_INCLUDE_IRQ_H
#define __CHIPS_E3650_INCLUDE_IRQ_H

#include <nuttx/config.h>
#include <arch/armv8-r/irq.h>

#define IDLE_STACK_BASE _core0_idle_stack_start
#define NR_IRQS 434U

#ifndef __ASSEMBLY__
void up_init_irq(int irq, int irq_prio);
bool up_irq_is_enabled(int irq);
void up_clear_irq(int irq);
uint32_t up_timer_freq(void);
int up_timer_value(clock_t *ticks);
uint32_t up_timer_gettickfreq(void);
#endif /* __ASSEMBLY__ */

#endif /* __CHIPS_E3650_INCLUDE_IRQ_H */
