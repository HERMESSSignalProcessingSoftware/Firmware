#ifndef TOOLS_H
#define TOOLS_H

#include <stdint.h>
#include "../sf2drivers/drivers/mss_gpio/mss_gpio.h"
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


/** Get GPIO input port
 *
 * Port must be initialized as input or input/output.
 * @param port The port number
 * @return true, if positive input, false otherwise
 */
inline bool getGpioInput (uint32_t port) {
    return (MSS_GPIO_get_inputs() >> port) & 1;
}


/** Convert 64bit integer to byte array
 *
 * Output is in big endianness
 * @param array The output array
 * @param val the input value
 */
void toByteArray (uint8_t array[8], uint64_t val);


/** Convert 16bit integer to byte array
 *
 * Output is in big endianness
 * @param array The output array
 * @param val the input value
 */
void toByteArray (uint8_t array[2], uint16_t val);


// All ISRs
extern "C" void Timer1_IRQHandler ();
extern "C" void Timer2_IRQHandler ();
extern "C" void F2M_INT_HANDLER(INT_STAMP1) ();
extern "C" void F2M_INT_HANDLER(INT_STAMP2) ();
extern "C" void F2M_INT_HANDLER(INT_STAMP3) ();
extern "C" void F2M_INT_HANDLER(INT_STAMP4) ();
extern "C" void F2M_INT_HANDLER(INT_STAMP5) ();
extern "C" void F2M_INT_HANDLER(INT_STAMP6) ();
extern "C" void F2M_INT_HANDLER(INT_TELEMETRY) ();

#endif
