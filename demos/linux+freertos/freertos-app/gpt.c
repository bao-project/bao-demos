/* gpt.c */
#include "gpt.h"
#include <stdio.h>

/* ── GPT1 registers ────────────────────────────────────────────────────── */
#define GPT1_BASE           0x302D0000UL
#define GPT_CR              (*(volatile uint32_t *)(GPT1_BASE + 0x00U))
#define GPT_PR              (*(volatile uint32_t *)(GPT1_BASE + 0x04U))
#define GPT_SR              (*(volatile uint32_t *)(GPT1_BASE + 0x08U))
#define GPT_IR              (*(volatile uint32_t *)(GPT1_BASE + 0x0CU))
#define GPT_OCR1            (*(volatile uint32_t *)(GPT1_BASE + 0x10U))
#define GPT_CNT             (*(volatile uint32_t *)(GPT1_BASE + 0x24U))

#define GPT_CR_EN           (1U << 0)
#define GPT_CR_ENMOD        (1U << 1)
#define GPT_CR_FRR          (1U << 9)
#define GPT_CR_EN_24M       (1U << 10)
#define GPT_CR_SWR          (1U << 15)
#define GPT_CR_CLKSRC(x)    ((x) << 6)
#define GPT_CLKSRC_24M      5U

#define GPT_SR_OF1          (1U << 0)
#define GPT_IR_OF1IE        (1U << 0)

#define GPT_CLOCK_HZ        100000000UL     /* 100MHz                  */
#define GPT_NS_PER_TICK     (1000000000UL / GPT_CLOCK_HZ)  /* 10ns    */
#define GPT_TICKS_PER_US    (GPT_CLOCK_HZ  / 1000000UL)    /* 100     */
/* ── Private state ─────────────────────────────────────────────────────── */
#define NULL ((void*)0)
static volatile gpt_callback_t s_cb           = NULL;
static volatile uint32_t       s_period_ticks = 0U;
static volatile uint32_t       s_ocr1_armed   = 0U;  /* When we expected to fire */

/* ── Public API ────────────────────────────────────────────────────────── */

void GPT_Init(void)
{
    GPT_CR = GPT_CR_SWR;
    while (GPT_CR & GPT_CR_SWR) {}

    GPT_PR = 0U;

    /*
     * CLKSRC=001: ipg_clk, fed by GPT1_CLK_ROOT (now enabled at 0x3038B580).
     * Drop EN_24M — that direct crystal path has quirks on i.MX8MP.
     * This is the path the NXP Linux driver uses.
     */
    GPT_CR = GPT_CR_CLKSRC(1U) |   /* ipg_clk from CCM root */
             GPT_CR_FRR         |
             GPT_CR_ENMOD;

    GPT_SR = 0x3FU;
    GPT_IR = 0U;

    GPT_CR |= GPT_CR_EN;
}

uint32_t GPT_GetTicks(void)
{
    return GPT_CNT;
}

uint32_t GPT_TicksToNs(uint32_t ticks)
{
    return ticks * GPT_NS_PER_TICK;
}

void GPT_DelayNanoseconds(uint32_t ns)
{
    uint32_t ticks = (ns + GPT_NS_PER_TICK - 1U) / GPT_NS_PER_TICK;
    uint32_t start = GPT_GetTicks();
    while ((GPT_GetTicks() - start) < ticks) {}
}

void GPT_DelayMicroseconds(uint32_t us)
{
    uint32_t start = GPT_GetTicks();
    while ((GPT_GetTicks() - start) < (us * GPT_TICKS_PER_US)) {}
}

void GPT_SetPeriodic(uint32_t period_us, gpt_callback_t cb)
{
    GPT_IR = 0U;

    s_period_ticks = period_us * GPT_TICKS_PER_US;
    s_cb           = cb;
    s_ocr1_armed   = GPT_CNT + s_period_ticks;

    GPT_OCR1 = s_ocr1_armed;
    GPT_SR   = GPT_SR_OF1;
    GPT_IR   = GPT_IR_OF1IE;
}

void GPT_Stop(void)
{
    GPT_IR = 0U;
    GPT_SR = GPT_SR_OF1;
    s_cb   = NULL;
}

void GPT_IRQHandler(void)
{
    /*
     * Read CNT immediately, but also snapshot OCR1 here.
     * OCR1 is a bus-domain register — no sync penalty.
     * CNT has a 2-cycle sync lag proportional to counter clock period.
     *
     * True latency ≈ cnt_on_entry − ocr1_snapshot
     *              + correction for CNT sync lag (2 × T_counter)
     */
    uint32_t cnt_on_entry  = GPT_CNT;
    uint32_t ocr1_snapshot = GPT_OCR1;   /* No sync cost */

    if (GPT_SR & GPT_SR_OF1)
    {
        GPT_SR = GPT_SR_OF1;

        uint32_t raw_latency      = cnt_on_entry - ocr1_snapshot;

        GPT_OCR1     = ocr1_snapshot + s_period_ticks;
        s_ocr1_armed = GPT_OCR1;

        gpt_callback_t cb = s_cb;
        if (cb != NULL)
        {
            cb(raw_latency);
        }
    }
}
