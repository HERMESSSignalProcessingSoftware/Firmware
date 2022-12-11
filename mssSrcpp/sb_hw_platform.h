#ifndef sb_HW_PLATFORM_H_
#define sb_HW_PLATFORM_H_


#define SPU_VERSION                     "1.3.1"

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
#define FLASH_CS_2                      GPIO_PORT(24)
#define FLASH_CS_1                      GPIO_PORT(25)
#define OUT_DAPI_CTS                    26
#define OUT_ADC_START                   28
#define LED_HB_MEMSYNC                  29
#define LED_RECORDING                   30
#define LED_HB_MSS                      31


/*-----------------------------------------------------------------------------
* CM3 subsystem memory map
* Initiator(s) for this subsystem: CM3 
*---------------------------------------------------------------------------*/
#define ADDR_STAMP1                     0x50000000U
#define ADDR_STAMP2                     0x50001000U
#define ADDR_STAMP3                     0x50002000U
#define ADDR_STAMP4                     0x50003000U
#define ADDR_STAMP5                     0x50004000U
#define ADDR_STAMP6                     0x50005000U
#define ADDR_TEMELEMTRY                 0x50006000U

/*-----------------------------------------------------------------------------
* F2M INTERRUPTS
*---------------------------------------------------------------------------*/
#define _F2M_INT_PIN(num)               FabricIrq##num##_IRQn
#define F2M_INT_PIN(num)                _F2M_INT_PIN(num)
#define _F2M_INT_HANDLER(num)           FabricIrq##num##_IRQHandler
#define F2M_INT_HANDLER(num)            _F2M_INT_HANDLER(num)

#define INT_STAMP1                      0
#define INT_STAMP2                      1
#define INT_STAMP3                      2
#define INT_STAMP4                      3
#define INT_STAMP5                      4
#define INT_STAMP6                      5
#define INT_TELEMETRY                   6

/*-----------------------------------------------------------------------------
* eNVM Addresses
*---------------------------------------------------------------------------*/
#define ADDR_NVM_CONF                   /*0x60032000U/**/0x32000U/**/


#endif /* sb_HW_PLATFORM_H_*/
