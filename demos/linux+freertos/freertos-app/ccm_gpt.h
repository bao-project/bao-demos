/* ccm_gpt.h */
#ifndef CCM_GPT_H
#define CCM_GPT_H

#include <stdint.h>

/*
 * GPT1 CCGR register in the i.MX8MP CCM.
 * Source: Linux kernel clk-imx8mp.c, ccm_base + 0x4B40.
 * CG field bits [1:0]:
 *   00 = clock off
 *   01 = clock on in run mode only
 *   11 = clock always on  ← we want this
 */
#define CCM_GPT1_CCGR   (*(volatile uint32_t *)(0x30384100))
#define CCM_CCGR_ON     0x3U

static inline void CCM_EnableGPT1(void)
{
    CCM_GPT1_CCGR = CCM_CCGR_ON;
}

/*
 * GPT1_CLK_ROOT TARGET_ROOT register.
 * CCM_BASE + 0x8000 + (root_index * 0x80)
 * GPT1 root index = 77 → offset 0xA680
 *
 * Bit 28:    ENABLE
 * Bits 26:24 MUX    → 0 = 24M_REF_CLK (matches our CLKSRC choice)
 * Bits 18:16 PRE_PODF  → 0 = divide by 1
 * Bits  5:0  POST_PODF → 0 = divide by 1
 */
#define CCM_GPT1_CLK_ROOT   (*(volatile uint32_t *)(0x3038b580UL))
#define CCM_ROOT_ENABLE     (1U << 28)
#define CCM_ROOT_MUX(x)     ((x) << 24)

static inline void CCM_EnableGPT1Root(void)
{
    /* Source from 24M_REF_CLK, no division, enabled */
    CCM_GPT1_CLK_ROOT = CCM_ROOT_ENABLE     |
                        CCM_ROOT_MUX(1U) |   /* sys_pll2_100m = 100MHz */
                        0U;                     /* POST/PRE div = 1 */
}

#endif /* CCM_GPT_H */
