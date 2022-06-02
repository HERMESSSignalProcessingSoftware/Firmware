#ifndef TOOLS_H
#define TOOLS_H

#include <stdint.h>
#include "../sb_hw_platform.h"

/**
 * Wait for at least the specified time in milliseconds. This function
 * introduces some overhead however and therefore waits longer than
 * the specified time. Also note, that other interrupts may significantly
 * impact the actual delay time.
 *
 * This function uses Timer1 and requires it to not be used by other functions.
 * This includes the 32 bit timer, which will not be usable simultaneously.
 *
 * @param ms
 * the time to wait in milliseconds
 */
void delay (uint32_t ms);

// All ISRs
extern "C" void Timer1_IRQHandler ();
extern "C" void Timer2_IRQHandler ();
extern "C" void F2M_INT_STAMP1_HANDLER ();
extern "C" void F2M_INT_STAMP2_HANDLER ();
extern "C" void F2M_INT_STAMP3_HANDLER ();
extern "C" void F2M_INT_STAMP4_HANDLER ();
extern "C" void F2M_INT_STAMP5_HANDLER ();
extern "C" void F2M_INT_STAMP6_HANDLER ();

#endif
