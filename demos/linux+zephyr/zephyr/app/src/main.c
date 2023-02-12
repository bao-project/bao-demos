#include <zephyr/kernel.h>
#include <zephyr/arch/cpu.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/virtualization/bao_ipcshmem.h>

#include <stdio.h>
#include <string.h>

#define MSG_SIZE 256

static const struct device *shmem = DEVICE_DT_GET(DT_INST(0, bao_ipcshmem));
static const struct device *uart = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

static struct k_thread thread1_data;
static struct k_thread thread2_data;

K_THREAD_STACK_DEFINE(thread1_stack_area, 1024);
K_THREAD_STACK_DEFINE(thread2_stack_area, 1024);

void thread(void *arg1, void *arg2, void *arg3) {

    size_t counter = 0;
    size_t id = (size_t) arg1;
    while (1) {
        unsigned cpu_id = arch_curr_cpu()->id;
        printk("Thread%ld (cpu%d): %ld\n", id, cpu_id, counter++);
        k_msleep(1000);
    }
}

void uart_rx_handler() {
    static size_t irq_count = 0;
    static char msg[MSG_SIZE];
    char c;
    unsigned cpu_id = arch_curr_cpu()->id;

    irq_count++;
    printk("%s (cpu%d): %ld\n", __func__, cpu_id, irq_count);
    while(uart_poll_in(uart, &c) == 0);
    snprintf(msg, MSG_SIZE,
        "zephyr has received %ld uart interrupts!\n", irq_count);
    bao_ipcshmem_write(shmem, msg, strnlen(msg, MSG_SIZE)+1);
}

void shmem_irq_handler(const struct device *dev) {
    static char msg[MSG_SIZE];
    bao_ipcshmem_read(dev, msg, MSG_SIZE);
    msg[MSG_SIZE-1] = '\0';
    char* end = strchr(msg, '\n');
    if (end != NULL) *end = '\0';
    printk("message from linux: %s\n", msg);
}

void main() {

    printk("Zephyr Bao Demo!\n");

    bao_ipcshmem_write(shmem, "", 1);
    bao_ipcshmem_irq_set_callback(shmem, 
        (bao_ipcshmem_callback_t)shmem_irq_handler);
    bao_ipcshmem_irq_enable(shmem);

    uart_irq_callback_set(uart, uart_rx_handler);
    uart_irq_rx_enable(uart);

	k_thread_create(&thread1_data, thread1_stack_area,
			K_THREAD_STACK_SIZEOF(thread1_stack_area),
			thread, (void*)1, NULL, NULL,
			1, 0, K_FOREVER);

	k_thread_create(&thread2_data, thread2_stack_area,
			K_THREAD_STACK_SIZEOF(thread2_stack_area),
			thread, (void*)2, NULL, NULL,
			1, 0, K_FOREVER);

	k_thread_start(&thread1_data);
	k_thread_start(&thread2_data);

}
