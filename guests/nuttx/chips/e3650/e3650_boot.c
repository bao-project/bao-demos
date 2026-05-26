#include <nuttx/config.h>
#include <nuttx/arch.h>
#include <nuttx/init.h>
#include <nuttx/irq.h>
#include <stdint.h>
#include "arm_internal.h"
#include "arch/chip/chip.h"

extern uint8_t _load_data_start[];
extern uint8_t _data_start[];
extern uint8_t _data_end[];

extern uint8_t _dma_buffer_start[];
extern uint8_t _dma_buffer_end[];

/* Per-CPU task/context storage for Bao guest mode.
 * Replaces CP15 TPIDRPRW, which is not accessible in guest VMs.
 */

uint32_t g_current_task_bao[CONFIG_SMP_NCPUS] = {0};

#ifdef CONFIG_SMP
static void e3650_boot_puts(const char *str)
{
  while (*str != '\0')
    {
      up_putc(*str++);
    }
}

static void e3650_boot_log_cpu_up(int cpu)
{
  e3650_boot_puts("NX cpu ");
  up_putc('0' + cpu);
  e3650_boot_puts(" up\n");
}
#endif

static inline_function void arm_custom_bss_init(void)
{
	memset(_dma_buffer_start, 0, (uintptr_t)_dma_buffer_end - (uintptr_t)_dma_buffer_start);
	up_flush_dcache((uintptr_t)_dma_buffer_start, (uintptr_t)_dma_buffer_end);
}

static inline_function void arm_custom_data_load(void)
{
	if ((uintptr_t)_load_data_start != (uintptr_t)_data_start) {
		memcpy(_data_start, _load_data_start, (uintptr_t)_data_end - (uintptr_t)_data_start);
	}
}

/****************************************************************************
 * Name: arm_el_init
 *
 * Description:
 *   The function called from arm_head.S at very early stage for these
 * platform, it's use to:
 *   - Handling special hardware initialize routine which is need to
 *     run at high ELs
 *   - Initialize system software such as hypervisor or security firmware
 *     which is need to run at high ELs
 *
 ****************************************************************************/

void arm_el_init(void)
{
	/* No high-EL initialization is required for this platform. */
}

/****************************************************************************
 * Name: arm_boot
 *
 * Description:
 *   Complete boot operations started in arm_head.S
 *
 ****************************************************************************/

void arm_boot(void)
{
	int cpu = up_cpu_index();

	if (cpu == 0) {
		/* One-time runtime image init (.data copy + custom bss clear). */
		arm_custom_data_load();
		arm_custom_bss_init();
	}

#ifdef CONFIG_SMP
	e3650_boot_log_cpu_up(cpu);
#endif
}
