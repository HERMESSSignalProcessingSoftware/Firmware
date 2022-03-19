#ifndef sb_HW_PLATFORM_H_
#define sb_HW_PLATFORM_H_



/*-----------------------------------------------------------------------------
* GPIO PORTS
*---------------------------------------------------------------------------*/
#define _GPIO_PORT(num)                 MSS_GPIO_##num
#define GPIO_PORT(num)                  _GPIO_PORT(num)
#define _GPIO_HANDLER(num)              GPIO##num##_IRQHandler
#define GPIO_HANDLER(num)               _GPIO_HANDLER(num)

#define IN_RXSM_LO                      0
#define IN_RXSM_SOE                     1
#define IN_RXSM_SODS                    2
#define IN_WP                           3
#define OUT_ADC_START                   28
#define LED_FPGA_LOADED                 29
#define LED_RECORDING                   30
#define LED_HB_MSS                      31


/*-----------------------------------------------------------------------------
* CM3 subsystem memory map
* Initiator(s) for this subsystem: CM3 
*---------------------------------------------------------------------------*/
#define ADDR_MEMSYNC                    0x50000000U
#define ADDR_STAMP1                     0x50001000U
#define ADDR_STAMP2                     0x50002000U
#define ADDR_STAMP3                     0x50003000U
#define ADDR_STAMP4                     0x50004000U
#define ADDR_STAMP5                     0x50005000U
#define ADDR_STAMP6                     0x50006000U
#define ADDR_TM                         0x50007000U

/*-----------------------------------------------------------------------------
* F2M INTERRUPTS (MSS_INT_F2M[x])
*---------------------------------------------------------------------------*/
#define F2M_INT_MEMSYNC                 0
#define F2M_INT_STAMP1                  1
#define F2M_INT_STAMP2                  2
#define F2M_INT_STAMP3                  3
#define F2M_INT_STAMP4                  4
#define F2M_INT_STAMP5                  5
#define F2M_INT_STAMP6                  6
#define F2M_INT_TM                      7


#endif /* sb_HW_PLATFORM_H_*/
