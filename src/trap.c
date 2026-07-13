#include <kernel/trap.h>
#include <kernel/panic.h>
#include <arch/csr.h>
#include <arch/plic.h>
#include <kernel/serial.h>
#include <arch/timer.h>

#define HART0 0

/* defined in src/trap_entry.S */
extern void trap_entry();

void handle_irq()
{
	u64 causa = csr_read(CSR_SCAUSE);
    switch (causa) {
    case TRAP_TIMER_IRQ:
        timer_irq();
        break;

    case TRAP_EXTERNAL_IRQ: {
        u32 irq = plic_hart_claim_irq(HART0);

        switch (irq) {
        case SERIAL_IRQ:
            serial_irq();
            break;
        case 0:
            break;
        default:
            break;
        }

        if (irq != 0) {
            plic_hart_complete_irq(HART0, irq);
        }
        break;
    }
    default:
        break;
    }
}

void handle_exception()
{
	u64 causa = csr_read(CSR_SCAUSE);
    u64 stval = csr_read(CSR_STVAL);
    u64 sepc  = csr_read(CSR_SEPC);

    switch (causa) {
    case EXCEPTION_INST_ACCESS_FAULT:
    case EXCEPTION_LOAD_ACCESS_FAULT:
    case EXCEPTION_STORE_ACCESS_FAULT:
    case EXCEPTION_INST_PAGE_FAULT:
    case EXCEPTION_LOAD_PAGE_FAULT:
    case EXCEPTION_STORE_PAGE_FAULT:
        error("page/access fault: cause=%llu stval(faulting addr)=%p sepc(faulting instr)=%p\n",
              causa, (void *)stval, (void *)sepc);
        break;
    default:
        error("unhandled exception: cause=%llu stval=%p sepc=%p\n",
              causa, (void *)stval, (void *)sepc);
        break;
    }

    BUG();
}

void trap_setup()
{
	// escreve o SVTEC 
	csr_write(CSR_STVEC, (u64)trap_entry);
	//não permite interrupção até o estado estar pronto
	csr_clear(CSR_SSTATUS, CSR_SSTATUS_SIE);
	csr_set(CSR_SIE, CSR_SIE_STIE);
}

void handle_trap()
{
	u64 causa = csr_read(CSR_SCAUSE);
	if(causa & TRAP_IRQ_BIT){
		handle_irq();
	}else{
		handle_exception();
	}
}

void hart_irq_enable()
{
	csr_set(CSR_SSTATUS, CSR_SSTATUS_SIE);
}

u64 hart_irq_save()
{
	u64 sstatus = csr_read(CSR_SSTATUS);
	csr_clear(CSR_SSTATUS, CSR_SSTATUS_SIE);
	return sstatus & CSR_SSTATUS_SIE;
}

void hart_irq_restore(u64 flags)
{
	if(flags & CSR_SSTATUS_SIE){
		csr_set(CSR_SSTATUS, CSR_SSTATUS_SIE);
	}
}

void hart_irq_disable()
{
	csr_clear(CSR_SSTATUS, CSR_SSTATUS_SIE);
}
