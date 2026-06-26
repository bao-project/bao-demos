#include <stdint.h>

#define WDOG1_BASE      0x30280000U
#define WDOG_WCR(base)  (*(volatile uint16_t *)((base) + 0x00))
#define WDOG_WSR(base)  (*(volatile uint16_t *)((base) + 0x02))
#define WDOG_WRSR(base) (*(volatile uint16_t *)((base) + 0x04))

#define WDOG_WCR_WDE    (1U << 2)   /* Watchdog Enable (write-once) */
#define WDOG_WCR_WT(x)  ((x) << 8) /* Timeout: (WT+1)*0.5s         */

static inline void WDOG_Kick(uint32_t base)
{
    WDOG_WSR(base) = 0x5555U;
    WDOG_WSR(base) = 0xAAAAU;
}

static inline void WDOG_SetTimeout(uint32_t base, uint8_t wt)
{
    uint16_t wcr = WDOG_WCR(base);
    wcr &= ~(0xFFU << 8);
    wcr |= WDOG_WCR_WT(wt);
    WDOG_WCR(base) = wcr;
    WDOG_Kick(base);  /* kick immediately after reconfigure */
}
