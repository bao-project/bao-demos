#ifndef __CHIPS_E3650_INCLUDE_CHIP_H
#define __CHIPS_E3650_INCLUDE_CHIP_H

#include <nuttx/config.h>
#include <stdint.h>

/* Number of bytes in x kibibytes/mebibytes/gibibytes */

#define KB(x)           ((x) << 10)
#define MB(x)           (KB(x) << 10)
#define GB(x)           (MB(UINT64_C(x)) << 10)

#if defined(CONFIG_ARCH_CHIP_E3650)

#define CONFIG_GICD_BASE         CONFIG_ARCH_CHIP_E3650_GICD_BASE
#define CONFIG_GICR_BASE         CONFIG_ARCH_CHIP_E3650_GICR_BASE
#define CONFIG_GICR_OFFSET       0x20000

#define MPID_TO_CLUSTER_ID(mpid)  ((mpid) & ~0xff)

#endif /* CONFIG_ARCH_CHIP_E3650 */

#endif /* __CHIPS_E3650_INCLUDE_CHIP_H */
