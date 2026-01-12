/** 
 *
 */

 #include <core.h>
 #include <stdlib.h>
 #include <stdio.h>
 #include <string.h> 
 #include <cpu.h>
 #include <wfi.h>
 #include <spinlock.h>
 #include <plat.h>
 #include <irq.h>
 #include <uart.h>
 #include <timer.h>
 
 #define TIMER_INTERVAL (TIME_S(1))
 
 spinlock_t print_lock = SPINLOCK_INITVAL;
 
 #define SHMEM_IRQ_ID (52)
 
char* const baremetal_message = (char*)SHMEM_BASE;
char* const zephyr_message    = (char*)(SHMEM_BASE + 0x2000);
 const size_t shmem_channel_size = 0x2000;
 
 void shmem_update_msg(int irq_count) {
     sprintf(baremetal_message, "Bao baremetal guest received %d uart interrupts!\n",
         irq_count);
 }
 
 char* strnchr(const char* s, size_t n, char c) {
     for (size_t i = 0; i < n; i++) {
         if (s[i] == c) {
             return (char*)s + i;
         }
     }
     return NULL;
 }
 
 void shmem_handler() {
     zephyr_message[shmem_channel_size-1] = '\0';
     char* end = strchr(zephyr_message, '\n');
     if (end != NULL) {
         *end = '\0';
     }
     printf("message from zephyr: %s\n", zephyr_message);
 }
 
 void shmem_init() {
     memset(baremetal_message, 0, shmem_channel_size);
     memset(zephyr_message, 0, shmem_channel_size);
     shmem_update_msg(0);
     irq_set_handler(SHMEM_IRQ_ID, shmem_handler);
     irq_set_prio(SHMEM_IRQ_ID, IRQ_MAX_PRIO);
     irq_enable(SHMEM_IRQ_ID);
 }
 
 void uart_rx_handler(){
     static int irq_count = 0;
     printf("cpu%d: %s %d\n",get_cpuid(), __func__, ++irq_count);
     uart_clear_rxirq();
         shmem_update_msg(irq_count);
 }
 
 void ipi_handler(){
     printf("cpu%d: %s\n", get_cpuid(), __func__);
     irq_send_ipi(1ull << (get_cpuid() + 1));
 }
 
 void timer_handler(){
     printf("cpu%d: %s\n", get_cpuid(), __func__);
     timer_set(TIMER_INTERVAL);
     irq_send_ipi(1ull << (get_cpuid() + 1));
 }
 
 void main(void){
 
     static volatile bool master_done = false;

     if(cpu_is_master()){
         spin_lock(&print_lock);
         printf("Bao bare-metal test guest!!!\n");
         spin_unlock(&print_lock);
 
         irq_set_handler(UART_IRQ_ID, uart_rx_handler);
         irq_set_handler(TIMER_IRQ_ID, timer_handler);
         irq_set_handler(IPI_IRQ_ID, ipi_handler);
 
         uart_enable_rxirq();
 
         timer_set(TIMER_INTERVAL);
         irq_enable(TIMER_IRQ_ID);
         irq_set_prio(TIMER_IRQ_ID, IRQ_MAX_PRIO);
 
         shmem_init();
 
         master_done = true;
     }
 
     irq_enable(UART_IRQ_ID);
     irq_set_prio(UART_IRQ_ID, IRQ_MAX_PRIO);
     irq_enable(IPI_IRQ_ID);
     irq_set_prio(IPI_IRQ_ID, IRQ_MAX_PRIO);
 
     while(!master_done);
     spin_lock(&print_lock);
     printf("cpu %d up\n", get_cpuid());
     spin_unlock(&print_lock);
 
     while(1) wfi();
 }
 
