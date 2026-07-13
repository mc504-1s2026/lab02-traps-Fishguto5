#include <kernel/serial.h>
#include <kernel/panic.h>
#include <arch/plic.h>
#include <arch/spinlock.h>

#define HART0 0

static char serial_buf[SERIAL_BUF_SIZE];
static size_t tam_serial = 0;
static struct spinlock serial_lock;

void serial_init()
{
	tam_serial = 0;
    spin_init(&serial_lock);

    *((volatile u8 *)SERIAL_BASE + SERIAL_FCR) =
        SERIAL_FCR_FIFO_ENABLE | SERIAL_FCR_RX_FIFO_CLEAR | SERIAL_FCR_TX_FIFO_CLEAR;

    *((volatile u8 *)SERIAL_BASE + SERIAL_IER) = SERIAL_IER_ERBFI;

    plic_irq_set_priority(IRQ_SERIAL, 1);
    plic_hart_set_threshold(HART0, 0);
    plic_hart_enable_irq(HART0, IRQ_SERIAL);
}

void serial_irq_enable()
{
	*((volatile u8 *)SERIAL_BASE + SERIAL_IER) |= SERIAL_IER_ERBFI;
}

void serial_irq_disable()
{
	*((volatile u8 *)SERIAL_BASE + SERIAL_IER) &= ~SERIAL_IER_ERBFI;
}

void serial_irq()
{
	spin_lock(&serial_lock);

    while (*((volatile u8 *)SERIAL_BASE + SERIAL_LSR) & SERIAL_LSR_DTR) {
        char ch = (char)*((volatile u8 *)SERIAL_BASE + SERIAL_RBR);
        if (tam_serial < SERIAL_BUF_SIZE) {
            serial_buf[tam_serial++] = ch;
        }
    }

    spin_unlock(&serial_lock);
}

size_t serial_read(char *buf)
{
	u64 flags = spin_lock_irqsave(&serial_lock);

    size_t size = tam_serial;
    for (size_t i = 0; i < size; i++) {
        buf[i] = serial_buf[i];
    }
    tam_serial = 0;

    spin_unlock_irqrestore(&serial_lock, flags);

    return size;
}

void serial_puts(char *str)
{
	 while (*str) {
        serial_putc(*str);
        str++;
    }
}

void serial_putc(char c)
{
	while (!(*((volatile u8 *)SERIAL_BASE + SERIAL_LSR) & SERIAL_LSR_THRE)) {
    }
    *((volatile u8 *)SERIAL_BASE + SERIAL_THR) = (u8)c;
}
