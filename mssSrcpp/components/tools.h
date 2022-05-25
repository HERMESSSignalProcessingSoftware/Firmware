#ifndef TOOLS_H
#define TOOLS_H

#include <stdint.h>

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

#endif
