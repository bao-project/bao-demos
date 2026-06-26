/* gpt.h */
#ifndef GPT_H
#define GPT_H

#include <stdint.h>

#define GPT1_GIC_ID     87U     /* SPI 55, level-sensitive, active high */

typedef void (*gpt_callback_t)(uint32_t latency_ticks);

void     GPT_Init(void);
uint32_t GPT_GetTicks(void);
uint32_t GPT_TicksToNs(uint32_t ticks);
void     GPT_DelayMicroseconds(uint32_t us);
void     GPT_DelayNanoseconds(uint32_t ns);
void     GPT_SetPeriodic(uint32_t period_us, gpt_callback_t cb);
void     GPT_Stop(void);
void GPT_IRQHandler();

#endif /* GPT_H */
