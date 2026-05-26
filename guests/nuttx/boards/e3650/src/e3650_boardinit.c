#include <nuttx/config.h>
#include "e3650.h"

/****************************************************************************
 * Name: board_late_initialize
 *
 * Description:
 *   If CONFIG_BOARD_LATE_INITIALIZE is selected, then an additional
 *   initialization call will be performed in the boot-up sequence to a
 *   function called board_late_initialize(). board_late_initialize() will be
 *   called immediately after up_intitialize() is called and just before the
 *   initial application is started.  This additional initialization phase
 *   may be used, for example, to initialize board-specific device drivers.
 *
 ****************************************************************************/

#ifdef CONFIG_BOARD_LATE_INITIALIZE
void board_late_initialize(void)
{
  /* Perform board initialization */

  e3650_bringup();
}
#endif /* CONFIG_BOARD_LATE_INITIALIZE */

#ifdef CONFIG_BOARDCTL
int board_app_initialize(uintptr_t arg)
{
  (void)arg;

#ifndef CONFIG_BOARD_LATE_INITIALIZE
  return e3650_bringup();
#else
  return 0;
#endif
}
#endif /* CONFIG_BOARDCTL */
