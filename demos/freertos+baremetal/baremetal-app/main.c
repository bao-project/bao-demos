/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#include <core.h>
#include <stdlib.h>
#include <stdio.h>
#include <cpu.h>
#include <wfi.h>
#include <spinlock.h>
#include <plat.h>
#include <irq.h>
#include <uart.h>
#include <timer.h>
#include <hypercall.h>

#define TIMER_INTERVAL (TIME_S(1))

#define shared_buff (*(volatile char *)0xfe480000UL)

extern volatile unsigned int uart_rxcnt;

spinlock_t print_lock = SPINLOCK_INITVAL;

void uart_rx_handler()
{
    uart_rxcnt++;
    
    printf("cpu%d: %s\n", get_cpuid(), __func__);
    
    shared_buff = uart_getchar();

    uart_clear_rxirq();
    bao_hypercall(1UL, 0, 0);
}

void ipi_handler()
{
    irq_clear_ipi();
    printf("cpu%d: %s\n", get_cpuid(), __func__);
    irq_send_ipi(1ull << (get_cpuid() + 1));
}

void timer_handler()
{
    printf("cpu%d: %s\n", get_cpuid(), __func__);
    timer_set(TIMER_INTERVAL);
    irq_send_ipi(1ull << (get_cpuid() + 1));
}

void main(void)
{
    static volatile bool master_done = false;

    if (cpu_is_master()) {

        shared_buff = '\0';

        spin_lock(&print_lock);
        printf("Bao bare-metal test guest\n");
        spin_unlock(&print_lock);

        irq_set_handler(UART_IRQ_ID, uart_rx_handler);
        irq_set_handler(TIMER_IRQ_ID, timer_handler);

        uart_enable_rxirq();

        timer_set(TIMER_INTERVAL);
        irq_enable(TIMER_IRQ_ID);
        irq_set_prio(TIMER_IRQ_ID, TIMER_IRQ_PRIO);

        irq_enable(UART_IRQ_ID);
        irq_set_prio(UART_IRQ_ID, UART_IRQ_PRIO);

        timer_enable();

        master_done = true;
    }

    irq_set_handler(IPI_IRQ_ID, ipi_handler);

    irq_enable(IPI_IRQ_ID);
    irq_set_prio(IPI_IRQ_ID, IPI_IRQ_PRIO);

    while (!master_done)
        ;
    spin_lock(&print_lock);
    printf("cpu %d up\n", get_cpuid());
    spin_unlock(&print_lock);

    while (1) {
        wfi();
    }
}
