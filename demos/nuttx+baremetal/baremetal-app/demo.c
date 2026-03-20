/**
 *
 */

#include <core.h>
#include <cpu.h>
#include <irq.h>
#include <spinlock.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SHMEM_MAGIC (0x424d5348u) /* "BMSH" */
#define SHMEM_MSG_LEN (96u)
#define SHMEM_HEARTBEAT_SPINS (5000000u)

struct shmem_mailbox {
    uint32_t magic;
    uint32_t baremetal_seq;
    uint32_t nuttx_seq;
    uint32_t ticks;
    char baremetal_msg[SHMEM_MSG_LEN];
    char nuttx_msg[SHMEM_MSG_LEN];
    char last_nuttx_msg[SHMEM_MSG_LEN];
};

static spinlock_t print_lock = SPINLOCK_INITVAL;
static spinlock_t master_lock = SPINLOCK_INITVAL;
static volatile struct shmem_mailbox *const g_mailbox =
    (volatile struct shmem_mailbox *)SHMEM_BASE;
static uint32_t g_seen_nuttx_seq;
static uint32_t g_poll_ticks;
static bool g_master_done;

static void shmem_update_status(void)
{
    snprintf((char *)g_mailbox->baremetal_msg, SHMEM_MSG_LEN,
             "BM alive: seq=%lu ticks=%lu cpu=%d",
             (unsigned long)g_mailbox->baremetal_seq,
             (unsigned long)g_mailbox->ticks, get_cpuid());
}

static void shmem_process_nuttx_msg(void)
{
    if (g_mailbox->nuttx_seq == g_seen_nuttx_seq) {
        return;
    }

    g_seen_nuttx_seq = g_mailbox->nuttx_seq;
    strncpy((char *)g_mailbox->last_nuttx_msg,
            (const char *)g_mailbox->nuttx_msg, SHMEM_MSG_LEN - 1);
    ((char *)g_mailbox->last_nuttx_msg)[SHMEM_MSG_LEN - 1] = '\0';

    g_mailbox->baremetal_seq++;
    snprintf((char *)g_mailbox->baremetal_msg, SHMEM_MSG_LEN,
             "BM ack seq=%lu msg=%s", (unsigned long)g_seen_nuttx_seq,
             (const char *)g_mailbox->last_nuttx_msg);

    spin_lock(&print_lock);
    printf("BM recv: %s\n", (const char *)g_mailbox->last_nuttx_msg);
    spin_unlock(&print_lock);
}

static void shmem_init(void)
{
    memset((void *)g_mailbox, 0, sizeof(*g_mailbox));
    g_mailbox->magic = SHMEM_MAGIC;
    g_mailbox->baremetal_seq = 1;
    snprintf((char *)g_mailbox->baremetal_msg, SHMEM_MSG_LEN,
             "BM ready on cpu %d", get_cpuid());
    g_seen_nuttx_seq = 0;
}

static void shmem_poll(void)
{
    if (get_cpuid() != 0) {
        return;
    }

    shmem_process_nuttx_msg();

    g_poll_ticks++;
    if (g_poll_ticks < SHMEM_HEARTBEAT_SPINS) {
        return;
    }

    g_poll_ticks = 0;
    g_mailbox->ticks++;

    if (g_mailbox->nuttx_seq == g_seen_nuttx_seq) {
        g_mailbox->baremetal_seq++;
        shmem_update_status();
    }
}

void main(void)
{
    bool master_done;

    if (cpu_is_master()) {
        spin_lock(&print_lock);
        printf("Bao bare-metal test guest\n");
        spin_unlock(&print_lock);

        shmem_init();

        spin_lock(&master_lock);
        g_master_done = true;
        spin_unlock(&master_lock);
    }

    do {
        spin_lock(&master_lock);
        master_done = g_master_done;
        spin_unlock(&master_lock);
    } while (!master_done);

    spin_lock(&print_lock);
    printf("cpu %d up\n", get_cpuid());
    spin_unlock(&print_lock);

    while (1) {
        shmem_poll();
    }
}
 
