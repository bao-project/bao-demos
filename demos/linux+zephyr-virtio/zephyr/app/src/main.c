/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Bao demo: Zephyr as a VirtIO console frontend.
 *
 * The VirtIO console is served by a backend running in another VM (Linux
 * with the Bao VirtIO device model). Two threads periodically write to the
 * VirtIO console, everything received from it is printed on the UART console,
 * and everything typed on the UART console is forwarded to the VirtIO console.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>

#define LINE_SIZE 128

static const struct device *const uart = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
static const struct device *const virtio_mmio = DEVICE_DT_GET(DT_NODELABEL(virtio_mmio0));
static const struct device *const vcon = DEVICE_DT_GET(DT_NODELABEL(virtio_console0));

/* Characters typed on the UART, waiting to be sent to the VirtIO console */
K_MSGQ_DEFINE(uart_rx_msgq, sizeof(char), 64, 1);

/* Serializes writes to the VirtIO console */
K_MUTEX_DEFINE(vcon_tx_lock);

static struct k_thread thread1_data;
static struct k_thread thread2_data;
static struct k_thread forward_thread_data;

K_THREAD_STACK_DEFINE(thread1_stack, 1024);
K_THREAD_STACK_DEFINE(thread2_stack, 1024);
K_THREAD_STACK_DEFINE(forward_thread_stack, 1024);

static void vcon_write(const char *s, size_t len)
{
	k_mutex_lock(&vcon_tx_lock, K_FOREVER);
	for (size_t i = 0; i < len; i++) {
		uart_poll_out(vcon, s[i]);
	}
	k_mutex_unlock(&vcon_tx_lock);
}

/* Periodically sends messages to the VirtIO console */
static void counter_thread(void *arg1, void *arg2, void *arg3)
{
	size_t id = (size_t)arg1;
	size_t counter = 0;
	char msg[32];

	while (1) {
		int len = snprintk(msg, sizeof(msg), "Thread%zu: %zu\r\n", id, counter++);

		vcon_write(msg, len);
		k_msleep(1000);
	}
}

/* Forwards characters typed on the UART to the VirtIO console */
static void forward_thread(void *arg1, void *arg2, void *arg3)
{
	char c;

	while (1) {
		k_msgq_get(&uart_rx_msgq, &c, K_FOREVER);
		vcon_write(&c, 1);
	}
}

/* Called on UART receive interrupts */
static void uart_rx_handler(const struct device *dev, void *user_data)
{
	char c;

	uart_irq_update(dev);

	while (uart_irq_rx_ready(dev) && uart_fifo_read(dev, &c, 1) == 1) {
		k_msgq_put(&uart_rx_msgq, &c, K_NO_WAIT);
	}
}

/* Called on VirtIO console receive interrupts: prints each received line */
static void vcon_rx_handler(const struct device *dev, void *user_data)
{
	static char line[LINE_SIZE];
	static size_t len;
	char c;

	while (uart_fifo_read(dev, &c, 1) == 1) {
		if (c == '\r' || c == '\n') {
			line[len] = '\0';
			printk("message from the virtio console: %s\n", line);
			len = 0;
		} else if (len < LINE_SIZE - 1) {
			line[len++] = c;
		}
	}
}

int main(void)
{
	printk("Bao Zephyr VirtIO console demo\n");

	/*
	 * The VirtIO MMIO accesses are forwarded by Bao to the backend VM, so
	 * initialization blocks until the backend (bao-virtio-dm) is running.
	 */
	printk("Initializing the virtio console (waiting for the backend)...\n");
	if (device_init(virtio_mmio) != 0 || device_init(vcon) != 0) {
		printk("virtio console initialization failed!\n");
		return -1;
	}
	printk("virtio console initialized\n");

	uart_irq_callback_set(vcon, vcon_rx_handler);
	uart_irq_rx_enable(vcon);

	uart_irq_callback_set(uart, uart_rx_handler);
	uart_irq_rx_enable(uart);

	k_thread_create(&forward_thread_data, forward_thread_stack,
			K_THREAD_STACK_SIZEOF(forward_thread_stack),
			forward_thread, NULL, NULL, NULL,
			1, 0, K_NO_WAIT);

	k_thread_create(&thread1_data, thread1_stack,
			K_THREAD_STACK_SIZEOF(thread1_stack),
			counter_thread, (void *)1, NULL, NULL,
			1, 0, K_NO_WAIT);

	k_thread_create(&thread2_data, thread2_stack,
			K_THREAD_STACK_SIZEOF(thread2_stack),
			counter_thread, (void *)2, NULL, NULL,
			1, 0, K_NO_WAIT);

	return 0;
}
