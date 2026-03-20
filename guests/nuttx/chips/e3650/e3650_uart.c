#include <nuttx/config.h>

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <nuttx/irq.h>
#include <nuttx/arch.h>
#include <nuttx/serial/serial.h>

#include "e3650_uart.h"

static int  e3650_uart_setup(struct uart_dev_s *dev);
static void e3650_uart_shutdown(struct uart_dev_s *dev);
static int  e3650_uart_attach(struct uart_dev_s *dev);
static void e3650_uart_detach(struct uart_dev_s *dev);
static int  e3650_uart_interrupt(int irq, void *context, void *arg);
static int  e3650_uart_ioctl(struct file *filep, int cmd, unsigned long arg);
static int  e3650_uart_receive(struct uart_dev_s *dev, unsigned int *status);
static void e3650_uart_rxint(struct uart_dev_s *dev, bool enable);
static bool e3650_uart_rxavailable(struct uart_dev_s *dev);
static void e3650_uart_send(struct uart_dev_s *dev, int ch);
static void e3650_uart_txint(struct uart_dev_s *dev, bool enable);
static bool e3650_uart_txready(struct uart_dev_s *dev);
static bool e3650_uart_txempty(struct uart_dev_s *dev);
static void e3650_uart_wait_txready(void);
static void e3650_uart_putc(int ch);

static const struct uart_ops_s g_uart_ops =
{
  .setup       = e3650_uart_setup,
  .shutdown    = e3650_uart_shutdown,
  .attach      = e3650_uart_attach,
  .detach      = e3650_uart_detach,
  .ioctl       = e3650_uart_ioctl,
  .receive     = e3650_uart_receive,
  .rxint       = e3650_uart_rxint,
  .rxavailable = e3650_uart_rxavailable,
  .send        = e3650_uart_send,
  .txint       = e3650_uart_txint,
  .txready     = e3650_uart_txready,
  .txempty     = e3650_uart_txempty,
};

static char g_rxbuf[64];
static char g_txbuf[64];

static struct uart_dev_s g_uart_dev =
{
  .isconsole = true,
  .ops       = &g_uart_ops,
  .xmit      = { .size = sizeof(g_txbuf), .buffer = g_txbuf },
  .recv      = { .size = sizeof(g_rxbuf), .buffer = g_rxbuf },
};

static void e3650_uart_wait_txready(void)
{
  volatile uint32_t *fsr0 = (volatile uint32_t *)(E3650_UART_BASE + E3650_UART_FSR0_OFF);

  while ((*fsr0 & E3650_UART_FSR0_TX_FULL) != 0)
    {
    }
}

static void e3650_uart_putc(int ch)
{
  volatile uint32_t *txdr = (volatile uint32_t *)(E3650_UART_BASE + E3650_UART_TXDR_OFF);

  e3650_uart_wait_txready();
  *txdr = (uint32_t)ch;
}

static int e3650_uart_setup(struct uart_dev_s *dev)
{
  return OK;
}

static void e3650_uart_shutdown(struct uart_dev_s *dev)
{
  up_disable_irq(E3650_UART_IRQ);
}

static int e3650_uart_attach(struct uart_dev_s *dev)
{
  int ret = irq_attach(E3650_UART_IRQ, e3650_uart_interrupt, dev);

  if (ret == OK)
    {
      volatile uint32_t *intr0 = (volatile uint32_t *)(E3650_UART_BASE +
                                                       E3650_UART_INTR0_OFF);

      /* Clear stale RX pending state before enabling the guest IRQ path. */

      *intr0 = E3650_UART_RX_INTMASK;
      up_init_irq(E3650_UART_IRQ, E3650_UART_IRQ_PRIO);
      up_enable_irq(E3650_UART_IRQ);
    }

  return ret;
}

static void e3650_uart_detach(struct uart_dev_s *dev)
{
  up_disable_irq(E3650_UART_IRQ);
  irq_detach(E3650_UART_IRQ);
}

static int e3650_uart_interrupt(int irq, void *context, void *arg)
{
  struct uart_dev_s *dev = (struct uart_dev_s *)arg;
  volatile uint32_t *intr0 = (volatile uint32_t *)(E3650_UART_BASE + E3650_UART_INTR0_OFF);
  uint32_t status = *intr0;

  if ((status & E3650_UART_RX_INTMASK) != 0 || e3650_uart_rxavailable(dev))
    {
      while (e3650_uart_rxavailable(dev))
        {
          uart_recvchars(dev);
        }
    }

  if ((status & E3650_UART_RX_INTMASK) != 0)
    {
      *intr0 = status & E3650_UART_RX_INTMASK;
    }

  return OK;
}

static int e3650_uart_ioctl(struct file *filep, int cmd, unsigned long arg)
{
  return -ENOTTY;
}

static int e3650_uart_receive(struct uart_dev_s *dev, unsigned int *status)
{
  volatile uint32_t *rxdr = (volatile uint32_t *)(E3650_UART_BASE + E3650_UART_RXDR_OFF);
  uint8_t ch = (uint8_t)(*rxdr & 0xff);

  if (ch == '\r')
    {
      ch = '\n';
    }

  *status = 0;
  return (int)ch;
}

static void e3650_uart_rxint(struct uart_dev_s *dev, bool enable)
{
  volatile uint32_t *inten0 = (volatile uint32_t *)(E3650_UART_BASE +
                                                    E3650_UART_INTEN0_OFF);
  volatile uint32_t *intr0 = (volatile uint32_t *)(E3650_UART_BASE + E3650_UART_INTR0_OFF);
  uint32_t val = *inten0;

  if (enable)
    {
      *intr0 = E3650_UART_RX_INTMASK;
      val |= E3650_UART_INTEN0_RXFWFE;
    }
  else
    {
      val &= ~E3650_UART_INTEN0_RXFWFE;
    }

  *inten0 = val;
}

static bool e3650_uart_rxavailable(struct uart_dev_s *dev)
{
  volatile uint32_t *fsr1 = (volatile uint32_t *)(E3650_UART_BASE + E3650_UART_FSR1_OFF);
  return (*fsr1 & E3650_UART_FSR1_RX_EMPTY) == 0;
}

static void e3650_uart_send(struct uart_dev_s *dev, int ch)
{
  (void)dev;
  e3650_uart_putc(ch);
}

static void e3650_uart_txint(struct uart_dev_s *dev, bool enable)
{
  if (enable)
    {
      uart_xmitchars(dev);
    }
}

static bool e3650_uart_txready(struct uart_dev_s *dev)
{
  /* TX progress is driven by the upper-half while send() performs the real
   * hardware backpressure check against the guest-visible FIFO state.
   */

  (void)dev;
  return true;
}

static bool e3650_uart_txempty(struct uart_dev_s *dev)
{
  return e3650_uart_txready(dev);
}

void up_earlyserialinit(void)
{
}

void up_serialinit(void)
{
}

void arm_earlyserialinit(void)
{
}

void arm_serialinit(void)
{
  uart_register("/dev/console", &g_uart_dev);
  uart_register("/dev/ttyS0", &g_uart_dev);
}

void up_putc(int ch)
{
  e3650_uart_putc(ch);
}
