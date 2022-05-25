#include "tools.h"
#include "../sf2drivers/drivers/mss_timer/mss_timer.h"
#include "../sf2drivers/drivers/mss_watchdog/mss_watchdog.h"


static volatile uint32_t delayCompleted = 0;
static volatile uint32_t delayCounter = 0;


void Timer1_IRQHandler () {
    if ((--delayCounter) < 1)
        delayCompleted = 1;
    MSS_TIM1_clear_irq();
}


void delay (uint32_t ms) {
    delayCompleted = 0;
    delayCounter = ms;
    MSS_TIM1_init(MSS_TIMER_PERIODIC_MODE);
    MSS_TIM1_load_immediate(100000); // 1ms @100MHz
    MSS_TIM1_enable_irq();
    MSS_TIM1_start();
    while (delayCompleted == 0) {
        MSS_WD_reload();
    }
    MSS_TIM1_stop();
}
