#include "tools.h"
#include "../sf2drivers/drivers/mss_timer/mss_timer.h"
#include "../sf2drivers/drivers/mss_gpio/mss_gpio.h"
#include "../sf2drivers/drivers/mss_watchdog/mss_watchdog.h"
#include "../components/controller.h"


static volatile uint32_t delayCompleted = 0;
static volatile uint32_t delayCounter = 0;


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



void toByteArray (uint8_t array[8], uint64_t val) {
    for (uint8_t i = 0; i < 8; i++) {
        array[i] = (uint8_t) ((val >> (8*(7-i))) & 0xFF);
    }
}



void toByteArray (uint8_t array[2], uint16_t val) {
    array[0] = (uint8_t) ((val >> 8) & 0xFFU);
    array[1] = (uint8_t) (val & 0xFFU);
}



extern "C" void Timer1_IRQHandler () {
    if ((--delayCounter) < 1)
        delayCompleted = 1;
    MSS_TIM1_clear_irq();
}


extern "C" void Timer2_IRQHandler () {
    Controller::getInstance().timestamp++;
    MSS_TIM2_clear_irq();
}

extern "C" void F2M_INT_HANDLER(INT_STAMP1) () {
    Measurement::getInstance().handleStampInterrupt(0);
}

extern "C" void F2M_INT_HANDLER(INT_STAMP2) () {
    Measurement::getInstance().handleStampInterrupt(1);
}

extern "C" void F2M_INT_HANDLER(INT_STAMP3) () {
    Measurement::getInstance().handleStampInterrupt(2);
}

extern "C" void F2M_INT_HANDLER(INT_STAMP4) () {
    Measurement::getInstance().handleStampInterrupt(3);
}

extern "C" void F2M_INT_HANDLER(INT_STAMP5) () {
    Measurement::getInstance().handleStampInterrupt(4);
}

extern "C" void F2M_INT_HANDLER(INT_STAMP6) () {
    Measurement::getInstance().handleStampInterrupt(5);
}
