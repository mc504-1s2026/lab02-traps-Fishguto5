#include <arch/timer.h>
#include <kernel/panic.h>
#include <arch/csr.h>

static volatile u64 uptime_secs = 0;
static u64 next_tick_time = 0;
static u64 alarm_target_time = 0;
static volatile bool alarm_fired = false;

u64 timer_read()
{
	return uptime_secs;
}

void timer_irq_enable()
{
	u64 mom = csr_read(CSR_TIME);
	next_tick_time = mom + TIMER_FREQ;
	csr_set(CSR_SIE, CSR_SIE_STIE);
	csr_write(CSR_STIMECMP, next_tick_time);
}

void timer_irq_disable()
{
	csr_clear(CSR_SIE, CSR_SIE_STIE);
}

void timer_set_alarm(u64 secs)
{
    u64 mom = csr_read(CSR_TIME);
    alarm_target_time = mom + secs * TIMER_FREQ;

    u64 next = next_tick_time;
    if (alarm_target_time != 0 && alarm_target_time < next) {
        next = alarm_target_time;
    }
    csr_write(CSR_STIMECMP, next);
}

void timer_irq()
{
	u64 mom = csr_read(CSR_TIME);

	if(mom >= next_tick_time){
		uptime_secs++;
		next_tick_time += TIMER_FREQ;
	}

	if (alarm_target_time != 0 && mom >= alarm_target_time){
		alarm_target_time = 0;
		alarm_fired = true;
	}

	u64 prox = next_tick_time;
	if(alarm_target_time != 0 && alarm_target_time < prox){
		prox = alarm_target_time;
	}
	csr_write(CSR_STIMECMP, prox);
}

bool timer_check_alarm()
{
    if (alarm_fired) {
        alarm_fired = false;
        return true;
    }
    return false;
}
