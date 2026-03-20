#include <nuttx/config.h>

#include <stdbool.h>
#include <stdint.h>

#include <nuttx/clock.h>

int up_timer_value(clock_t *ticks);

/****************************************************************************
 * Name: up_udelay
 *
 * Description:
 *   Provide a simple busy-wait delay using the platform timer value.
 *
 ****************************************************************************/

void up_udelay(uint32_t us)
{
  clock_t start;
  clock_t end;

  if (us == 0)
    {
      return;
    }

  up_timer_value(&start);
  end = start + ((clock_t)us * CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC / 1000000);

  do
    {
      up_timer_value(&start);
    }
  while (start < end);
}
