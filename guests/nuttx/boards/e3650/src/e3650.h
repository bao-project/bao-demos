#ifndef __BOARDS_E3650_SRC_E3650_H
#define __BOARDS_E3650_SRC_E3650_H

#include <nuttx/config.h>
#include <stdint.h>

#ifndef __ASSEMBLY__

/****************************************************************************
 * Name: e3650_bringup
 *
 * Description:
 *   Bring up board features
 *
 ****************************************************************************/

#if defined(CONFIG_BOARDCTL) || defined(CONFIG_BOARD_LATE_INITIALIZE)
int e3650_bringup(void);
#endif

#endif /* __ASSEMBLY__ */
#endif /* __BOARDS_E3650_SRC_E3650_H */
