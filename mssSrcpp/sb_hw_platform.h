#ifndef sb_HW_PLATFORM_H_
#define sb_HW_PLATFORM_H_



/*-----------------------------------------------------------------------------
* GPIO PORTS
*---------------------------------------------------------------------------*/
#define _GPIO_PORT(num)                 MSS_GPIO_##num
#define GPIO_PORT(num)                  _GPIO_PORT(num)
#define _GPIO_HANDLER(num)              GPIO##num##_IRQHandler
#define GPIO_HANDLER(num)               _GPIO_HANDLER(num)

#define IN_RXSM_LO                      MSS_GPIO_0
#define IN_RXSM_SOE                     MSS_GPIO_1
#define IN_RXSM_SODS                    MSS_GPIO_2
#define IN_WP                           MSS_GPIO_3
#define OUT_ADC_START                   MSS_GPIO_28
#define LED_FPGA_LOADED                 MSS_GPIO_29
#define LED_RECORDING                   MSS_GPIO_30
#define LED_HB_MSS                      MSS_GPIO_31


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
#define ADDR_TM                         0x50006000U

/*-----------------------------------------------------------------------------
* F2M INTERRUPTS
*---------------------------------------------------------------------------*/
#define F2M_INT_MEMSYNC_PIN             FabricIrq0_IRQn
#define F2M_INT_MEMSYNC_HANDLER         FabricIrq0_IRQHandler
#define F2M_INT_STAMP1_PIN              FabricIrq1_IRQn
#define F2M_INT_STAMP1_HANDLER          FabricIrq1_IRQHandler
#define F2M_INT_STAMP2_PIN              FabricIrq2_IRQn
#define F2M_INT_STAMP2_HANDLER          FabricIrq2_IRQHandler
#define F2M_INT_STAMP3_PIN              FabricIrq3_IRQn
#define F2M_INT_STAMP3_HANDLER          FabricIrq3_IRQHandler
#define F2M_INT_STAMP4_PIN              FabricIrq4_IRQn
#define F2M_INT_STAMP4_HANDLER          FabricIrq4_IRQHandler
#define F2M_INT_STAMP5_PIN              FabricIrq5_IRQn
#define F2M_INT_STAMP5_HANDLER          FabricIrq5_IRQHandler
#define F2M_INT_STAMP6_PIN              FabricIrq6_IRQn
#define F2M_INT_STAMP6_HANDLER          FabricIrq6_IRQHandler
#define F2M_INT_TM_PIN                  FabricIrq7_IRQn
#define F2M_INT_TM_HANDLER              FabricIrq7_IRQHandler


#endif /* sb_HW_PLATFORM_H_*/
