#include <kernel/printf.h>
#include <kernel/mm.h>
#include <arch/timer.h>
#include <kernel/trap.h>
#include <kernel/serial.h>
#include <kernel/string.h>
#include <arch/csr.h>

extern int _hartid[];
#define MAX 128

static void print_u64(u64 val)
{
    char buf[21];
    int i = 20;
    buf[i] = '\0';

    if (val == 0) {
        buf[--i] = '0';
    } else {
        while (val > 0) {
            buf[--i] = (char)('0' + (val % 10));
            val /= 10;
        }
    }

    serial_puts(&buf[i]);
}

static void shell_exec(char *line)
{
    if (strlen(line) == 0) {
        return;
    }

    if (strcmp(line, "uptime") == 0) {
        print_u64(timer_read());
        serial_puts("s\n");
    } else if (strncmp(line, "echo ", 5) == 0) {
        serial_puts(line + 5);
        serial_puts("\n");
    } else if (strncmp(line, "alarm ", 6) == 0) {
        u64 secs = strtou64(line + 6, 10);
        timer_set_alarm(secs);
    } else {
        serial_puts("unknown command\n");
    }
}

void kmain()
{
	printk_set_level(LOG_DEBUG);
	info("entered S-mode\n");
	info("booting on hart %d\n", _hartid[0]);
	info("setting up virtual memory...\n");
	vm_init();
	info("enabling traps...\n");
	trap_setup();
	info("enabling timer...\n");
	timer_irq_enable();
	info("enabling serial...\n");
	serial_init();
	serial_irq_enable();

	csr_set(CSR_SIE, CSR_SIE_SEIE);
    hart_irq_enable();

	char line[MAX];
    size_t line_len = 0;
	serial_puts("> ");


	while (1){
		if (timer_check_alarm()) {
            serial_puts("alarm\n> ");
        }

        char chunk[SERIAL_BUF_SIZE];
        size_t n = serial_read(chunk);

        for (size_t i = 0; i < n; i++) {
            char c = chunk[i];

            if (c == '\r' || c == '\n') {
                serial_putc('\n');
                line[line_len] = '\0';
                shell_exec(line);
                line_len = 0;
                serial_puts("> ");
            } else {
                serial_putc(c);
                if (line_len < MAX - 1) {
                    line[line_len++] = c;
                }
            }
        }
    }
}		
