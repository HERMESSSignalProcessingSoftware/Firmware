----------------------------------------------------------------------
-- Created by SmartDesign Wed Jun 15 17:58:17 2022
-- Version: 2021.3 2021.3.0.10
----------------------------------------------------------------------

----------------------------------------------------------------------
-- Libraries
----------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;

library smartfusion2;
use smartfusion2.all;
----------------------------------------------------------------------
-- root entity declaration
----------------------------------------------------------------------
entity root is
    -- Port list
    port(
        -- Inputs
        DAPI_RXD         : in  std_logic;
        DEVRST_N         : in  std_logic;
        F_MISO           : in  std_logic;
        IN_RXSM_LO       : in  std_logic;
        IN_RXSM_SODS     : in  std_logic;
        IN_RXSM_SOE      : in  std_logic;
        IN_WP            : in  std_logic;
        STAMP1_DRDY_SGR1 : in  std_logic;
        STAMP1_DRDY_SGR2 : in  std_logic;
        STAMP1_DRDY_TEMP : in  std_logic;
        STAMP1_MISO      : in  std_logic;
        STAMP2_DRDY_SGR1 : in  std_logic;
        STAMP2_DRDY_SGR2 : in  std_logic;
        STAMP2_DRDY_TEMP : in  std_logic;
        STAMP2_MISO      : in  std_logic;
        STAMP3_DRDY_SGR1 : in  std_logic;
        STAMP3_DRDY_SGR2 : in  std_logic;
        STAMP3_DRDY_TEMP : in  std_logic;
        STAMP3_MISO      : in  std_logic;
        STAMP4_DRDY_SGR1 : in  std_logic;
        STAMP4_DRDY_SGR2 : in  std_logic;
        STAMP4_DRDY_TEMP : in  std_logic;
        STAMP4_MISO      : in  std_logic;
        STAMP5_DRDY_SGR1 : in  std_logic;
        STAMP5_DRDY_SGR2 : in  std_logic;
        STAMP5_DRDY_TEMP : in  std_logic;
        STAMP5_MISO      : in  std_logic;
        STAMP6_DRDY_SGR1 : in  std_logic;
        STAMP6_DRDY_SGR2 : in  std_logic;
        STAMP6_DRDY_TEMP : in  std_logic;
        STAMP6_MISO      : in  std_logic;
        TM_RXD           : in  std_logic;
        -- Outputs
        DAPI_RTS         : out std_logic;
        DAPI_TXD         : out std_logic;
        F_CLK            : out std_logic;
        F_CS1            : out std_logic;
        F_CS2            : out std_logic;
        F_MOSI           : out std_logic;
        LED_FPGA_LOADED  : out std_logic;
        LED_HB_MEMSYNC   : out std_logic;
        LED_HB_MSS       : out std_logic;
        LED_RECORDING    : out std_logic;
        OUT_ADC_START    : out std_logic;
        STAMP1_CS_SGR1   : out std_logic;
        STAMP1_CS_SGR2   : out std_logic;
        STAMP1_CS_TEMP   : out std_logic;
        STAMP1_MOSI      : out std_logic;
        STAMP1_SCLK      : out std_logic;
        STAMP2_CS_SGR1   : out std_logic;
        STAMP2_CS_SGR2   : out std_logic;
        STAMP2_CS_TEMP   : out std_logic;
        STAMP2_MOSI      : out std_logic;
        STAMP2_SCLK      : out std_logic;
        STAMP3_CS_SGR1   : out std_logic;
        STAMP3_CS_SGR2   : out std_logic;
        STAMP3_CS_TEMP   : out std_logic;
        STAMP3_MOSI      : out std_logic;
        STAMP3_SCLK      : out std_logic;
        STAMP4_CS_SGR1   : out std_logic;
        STAMP4_CS_SGR2   : out std_logic;
        STAMP4_CS_TEMP   : out std_logic;
        STAMP4_MOSI      : out std_logic;
        STAMP4_SCLK      : out std_logic;
        STAMP5_CS_SGR1   : out std_logic;
        STAMP5_CS_SGR2   : out std_logic;
        STAMP5_CS_TEMP   : out std_logic;
        STAMP5_MOSI      : out std_logic;
        STAMP5_SCLK      : out std_logic;
        STAMP6_CS_SGR1   : out std_logic;
        STAMP6_CS_SGR2   : out std_logic;
        STAMP6_CS_TEMP   : out std_logic;
        STAMP6_MOSI      : out std_logic;
        STAMP6_SCLK      : out std_logic;
        TM_TXD           : out std_logic
        );
end root;
----------------------------------------------------------------------
-- root architecture body
----------------------------------------------------------------------
architecture RTL of root is
----------------------------------------------------------------------
-- Component declarations
----------------------------------------------------------------------
-- AND3
component AND3
    -- Port list
    port(
        -- Inputs
        A : in  std_logic;
        B : in  std_logic;
        C : in  std_logic;
        -- Outputs
        Y : out std_logic
        );
end component;
-- root_sb
component root_sb
    -- Port list
    port(
        -- Inputs
        DEVRST_N           : in  std_logic;
        FAB_RESET_N        : in  std_logic;
        GPIO_0_F2M         : in  std_logic;
        GPIO_1_F2M         : in  std_logic;
        GPIO_2_F2M         : in  std_logic;
        GPIO_3_F2M         : in  std_logic;
        MMUART_0_RXD_F2M   : in  std_logic;
        SPI_0_CLK_F2M      : in  std_logic;
        SPI_0_DI_F2M       : in  std_logic;
        SPI_0_SS0_F2M      : in  std_logic;
        STAMP_0_INTR_0_top : in  std_logic;
        STAMP_1_INTR_0_top : in  std_logic;
        STAMP_1_PRDATAS1   : in  std_logic_vector(31 downto 0);
        STAMP_1_PREADYS1   : in  std_logic;
        STAMP_1_PSLVERRS1  : in  std_logic;
        STAMP_2_INTR_0_top : in  std_logic;
        STAMP_2_PRDATAS2   : in  std_logic_vector(31 downto 0);
        STAMP_2_PREADYS2   : in  std_logic;
        STAMP_2_PSLVERRS2  : in  std_logic;
        STAMP_3_INTR_0_top : in  std_logic;
        STAMP_3_PRDATAS3   : in  std_logic_vector(31 downto 0);
        STAMP_3_PREADYS3   : in  std_logic;
        STAMP_3_PSLVERRS3  : in  std_logic;
        STAMP_4_INTR_0_top : in  std_logic;
        STAMP_4_PRDATAS4   : in  std_logic_vector(31 downto 0);
        STAMP_4_PREADYS4   : in  std_logic;
        STAMP_4_PSLVERRS4  : in  std_logic;
        STAMP_5_INTR_0_top : in  std_logic;
        STAMP_5_PRDATAS5   : in  std_logic_vector(31 downto 0);
        STAMP_5_PREADYS5   : in  std_logic;
        STAMP_5_PSLVERRS5  : in  std_logic;
        STAMP_PRDATAS0     : in  std_logic_vector(31 downto 0);
        STAMP_PREADYS0     : in  std_logic;
        STAMP_PSLVERRS0    : in  std_logic;
        TM_0_INTR_0_top    : in  std_logic;
        TM_PRDATAS6        : in  std_logic_vector(31 downto 0);
        TM_PREADYS6        : in  std_logic;
        TM_PSLVERRS6       : in  std_logic;
        -- Outputs
        FIC_0_CLK          : out std_logic;
        FIC_0_LOCK         : out std_logic;
        GPIO_26_M2F        : out std_logic;
        GPIO_28_M2F        : out std_logic;
        GPIO_29_M2F        : out std_logic;
        GPIO_30_M2F        : out std_logic;
        GPIO_31_M2F        : out std_logic;
        INIT_DONE          : out std_logic;
        MMUART_0_TXD_M2F   : out std_logic;
        MSS_READY          : out std_logic;
        POWER_ON_RESET_N   : out std_logic;
        SPI_0_CLK_M2F      : out std_logic;
        SPI_0_DO_M2F       : out std_logic;
        SPI_0_SS0_M2F      : out std_logic;
        SPI_0_SS0_M2F_OE   : out std_logic;
        SPI_0_SS1_M2F      : out std_logic;
        STAMP_1_PADDRS     : out std_logic_vector(31 downto 0);
        STAMP_1_PENABLES   : out std_logic;
        STAMP_1_PSELS1     : out std_logic;
        STAMP_1_PWDATAS    : out std_logic_vector(31 downto 0);
        STAMP_1_PWRITES    : out std_logic;
        STAMP_2_PADDRS     : out std_logic_vector(31 downto 0);
        STAMP_2_PENABLES   : out std_logic;
        STAMP_2_PSELS2     : out std_logic;
        STAMP_2_PWDATAS    : out std_logic_vector(31 downto 0);
        STAMP_2_PWRITES    : out std_logic;
        STAMP_3_PADDRS     : out std_logic_vector(31 downto 0);
        STAMP_3_PENABLES   : out std_logic;
        STAMP_3_PSELS3     : out std_logic;
        STAMP_3_PWDATAS    : out std_logic_vector(31 downto 0);
        STAMP_3_PWRITES    : out std_logic;
        STAMP_4_PADDRS     : out std_logic_vector(31 downto 0);
        STAMP_4_PENABLES   : out std_logic;
        STAMP_4_PSELS4     : out std_logic;
        STAMP_4_PWDATAS    : out std_logic_vector(31 downto 0);
        STAMP_4_PWRITES    : out std_logic;
        STAMP_5_PADDRS     : out std_logic_vector(31 downto 0);
        STAMP_5_PENABLES   : out std_logic;
        STAMP_5_PSELS5     : out std_logic;
        STAMP_5_PWDATAS    : out std_logic_vector(31 downto 0);
        STAMP_5_PWRITES    : out std_logic;
        STAMP_PADDRS       : out std_logic_vector(31 downto 0);
        STAMP_PENABLES     : out std_logic;
        STAMP_PSELS0       : out std_logic;
        STAMP_PWDATAS      : out std_logic_vector(31 downto 0);
        STAMP_PWRITES      : out std_logic;
        TM_PADDRS          : out std_logic_vector(31 downto 0);
        TM_PENABLES        : out std_logic;
        TM_PSELS6          : out std_logic;
        TM_PWDATAS         : out std_logic_vector(31 downto 0);
        TM_PWRITES         : out std_logic
        );
end component;
-- STAMP
-- using entity instantiation for component STAMP
----------------------------------------------------------------------
-- Signal declarations
----------------------------------------------------------------------
signal DAPI_RTS_net_0        : std_logic;
signal DAPI_TXD_net_0        : std_logic;
signal F_CLK_net_0           : std_logic;
signal F_CS1_net_0           : std_logic;
signal F_CS2_net_0           : std_logic;
signal F_MOSI_net_0          : std_logic;
signal LED_FPGA_LOADED_net_0 : std_logic;
signal LED_HB_MEMSYNC_net_0  : std_logic;
signal LED_HB_MSS_net_0      : std_logic;
signal LED_RECORDING_net_0   : std_logic;
signal MSS_FIC_0_CLK         : std_logic;
signal MSS_INIT_DONE         : std_logic;
signal MSS_MSS_READY         : std_logic;
signal MSS_POWER_ON_RESET_N  : std_logic;
signal MSS_STAMP_PENABLE     : std_logic;
signal MSS_STAMP_PRDATA      : std_logic_vector(31 downto 0);
signal MSS_STAMP_PREADY      : std_logic;
signal MSS_STAMP_PSELx       : std_logic;
signal MSS_STAMP_PSLVERR     : std_logic;
signal MSS_STAMP_PWDATA      : std_logic_vector(31 downto 0);
signal MSS_STAMP_PWRITE      : std_logic;
signal MSS_STAMP_1_PENABLE   : std_logic;
signal MSS_STAMP_1_PRDATA    : std_logic_vector(31 downto 0);
signal MSS_STAMP_1_PREADY    : std_logic;
signal MSS_STAMP_1_PSELx     : std_logic;
signal MSS_STAMP_1_PSLVERR   : std_logic;
signal MSS_STAMP_1_PWDATA    : std_logic_vector(31 downto 0);
signal MSS_STAMP_1_PWRITE    : std_logic;
signal MSS_STAMP_2_PENABLE   : std_logic;
signal MSS_STAMP_2_PRDATA    : std_logic_vector(31 downto 0);
signal MSS_STAMP_2_PREADY    : std_logic;
signal MSS_STAMP_2_PSELx     : std_logic;
signal MSS_STAMP_2_PSLVERR   : std_logic;
signal MSS_STAMP_2_PWDATA    : std_logic_vector(31 downto 0);
signal MSS_STAMP_2_PWRITE    : std_logic;
signal MSS_STAMP_3_PENABLE   : std_logic;
signal MSS_STAMP_3_PRDATA    : std_logic_vector(31 downto 0);
signal MSS_STAMP_3_PREADY    : std_logic;
signal MSS_STAMP_3_PSELx     : std_logic;
signal MSS_STAMP_3_PSLVERR   : std_logic;
signal MSS_STAMP_3_PWDATA    : std_logic_vector(31 downto 0);
signal MSS_STAMP_3_PWRITE    : std_logic;
signal MSS_STAMP_4_PENABLE   : std_logic;
signal MSS_STAMP_4_PRDATA    : std_logic_vector(31 downto 0);
signal MSS_STAMP_4_PREADY    : std_logic;
signal MSS_STAMP_4_PSELx     : std_logic;
signal MSS_STAMP_4_PSLVERR   : std_logic;
signal MSS_STAMP_4_PWDATA    : std_logic_vector(31 downto 0);
signal MSS_STAMP_4_PWRITE    : std_logic;
signal MSS_STAMP_5_PENABLE   : std_logic;
signal MSS_STAMP_5_PRDATA    : std_logic_vector(31 downto 0);
signal MSS_STAMP_5_PREADY    : std_logic;
signal MSS_STAMP_5_PSELx     : std_logic;
signal MSS_STAMP_5_PSLVERR   : std_logic;
signal MSS_STAMP_5_PWDATA    : std_logic_vector(31 downto 0);
signal MSS_STAMP_5_PWRITE    : std_logic;
signal OUT_ADC_START_net_0   : std_logic;
signal STAMP1_CS_SGR1_net_0  : std_logic;
signal STAMP1_CS_SGR2_net_0  : std_logic;
signal STAMP1_CS_TEMP_net_0  : std_logic;
signal STAMP1_MOSI_net_0     : std_logic;
signal STAMP1_new_avail      : std_logic;
signal STAMP1_SCLK_net_0     : std_logic;
signal STAMP2_CS_SGR1_net_0  : std_logic;
signal STAMP2_CS_SGR2_net_0  : std_logic;
signal STAMP2_CS_TEMP_net_0  : std_logic;
signal STAMP2_MOSI_net_0     : std_logic;
signal STAMP2_new_avail      : std_logic;
signal STAMP2_SCLK_net_0     : std_logic;
signal STAMP3_CS_SGR1_net_0  : std_logic;
signal STAMP3_CS_SGR2_net_0  : std_logic;
signal STAMP3_CS_TEMP_net_0  : std_logic;
signal STAMP3_MOSI_net_0     : std_logic;
signal STAMP3_new_avail      : std_logic;
signal STAMP3_SCLK_net_0     : std_logic;
signal STAMP4_CS_SGR1_net_0  : std_logic;
signal STAMP4_CS_SGR2_net_0  : std_logic;
signal STAMP4_CS_TEMP_net_0  : std_logic;
signal STAMP4_MOSI_net_0     : std_logic;
signal STAMP4_new_avail      : std_logic;
signal STAMP4_SCLK_net_0     : std_logic;
signal STAMP5_CS_SGR1_net_0  : std_logic;
signal STAMP5_CS_SGR2_net_0  : std_logic;
signal STAMP5_CS_TEMP_net_0  : std_logic;
signal STAMP5_MOSI_net_0     : std_logic;
signal STAMP5_new_avail      : std_logic;
signal STAMP5_SCLK_net_0     : std_logic;
signal STAMP6_CS_SGR1_net_0  : std_logic;
signal STAMP6_CS_SGR2_net_0  : std_logic;
signal STAMP6_CS_TEMP_net_0  : std_logic;
signal STAMP6_MOSI_net_0     : std_logic;
signal STAMP6_new_avail      : std_logic;
signal STAMP6_SCLK_net_0     : std_logic;
signal DAPI_TXD_net_1        : std_logic;
signal F_CLK_net_1           : std_logic;
signal F_CS1_net_1           : std_logic;
signal F_CS2_net_1           : std_logic;
signal F_MOSI_net_1          : std_logic;
signal LED_HB_MSS_net_1      : std_logic;
signal LED_RECORDING_net_1   : std_logic;
signal OUT_ADC_START_net_1   : std_logic;
signal LED_FPGA_LOADED_net_1 : std_logic;
signal STAMP6_CS_SGR2_net_1  : std_logic;
signal STAMP6_CS_SGR1_net_1  : std_logic;
signal STAMP6_MOSI_net_1     : std_logic;
signal STAMP6_SCLK_net_1     : std_logic;
signal STAMP6_CS_TEMP_net_1  : std_logic;
signal STAMP1_CS_TEMP_net_1  : std_logic;
signal STAMP1_MOSI_net_1     : std_logic;
signal STAMP1_SCLK_net_1     : std_logic;
signal STAMP1_CS_SGR1_net_1  : std_logic;
signal STAMP1_CS_SGR2_net_1  : std_logic;
signal STAMP2_CS_SGR2_net_1  : std_logic;
signal STAMP2_SCLK_net_1     : std_logic;
signal STAMP2_CS_SGR1_net_1  : std_logic;
signal STAMP2_MOSI_net_1     : std_logic;
signal STAMP2_CS_TEMP_net_1  : std_logic;
signal STAMP4_CS_SGR1_net_1  : std_logic;
signal STAMP4_CS_SGR2_net_1  : std_logic;
signal STAMP4_MOSI_net_1     : std_logic;
signal STAMP4_SCLK_net_1     : std_logic;
signal STAMP4_CS_TEMP_net_1  : std_logic;
signal STAMP5_MOSI_net_1     : std_logic;
signal STAMP5_CS_SGR1_net_1  : std_logic;
signal STAMP5_CS_SGR2_net_1  : std_logic;
signal STAMP5_SCLK_net_1     : std_logic;
signal STAMP5_CS_TEMP_net_1  : std_logic;
signal STAMP3_CS_SGR2_net_1  : std_logic;
signal STAMP3_SCLK_net_1     : std_logic;
signal STAMP3_CS_SGR1_net_1  : std_logic;
signal STAMP3_MOSI_net_1     : std_logic;
signal STAMP3_CS_TEMP_net_1  : std_logic;
signal LED_HB_MEMSYNC_net_1  : std_logic;
signal TM_RXD_net_0          : std_logic;
signal DAPI_RTS_net_1        : std_logic;
----------------------------------------------------------------------
-- TiedOff Signals
----------------------------------------------------------------------
signal GND_net               : std_logic;
signal VCC_net               : std_logic;
signal TM_PRDATAS6_const_net_0: std_logic_vector(31 downto 0);
----------------------------------------------------------------------
-- Bus Interface Nets Declarations - Unequal Pin Widths
----------------------------------------------------------------------
signal MSS_STAMP_PADDR       : std_logic_vector(31 downto 0);
signal MSS_STAMP_PADDR_0     : std_logic_vector(11 downto 0);
signal MSS_STAMP_PADDR_0_11to0: std_logic_vector(11 downto 0);

signal MSS_STAMP_1_PADDR     : std_logic_vector(31 downto 0);
signal MSS_STAMP_1_PADDR_0   : std_logic_vector(11 downto 0);
signal MSS_STAMP_1_PADDR_0_11to0: std_logic_vector(11 downto 0);

signal MSS_STAMP_2_PADDR     : std_logic_vector(31 downto 0);
signal MSS_STAMP_2_PADDR_0   : std_logic_vector(11 downto 0);
signal MSS_STAMP_2_PADDR_0_11to0: std_logic_vector(11 downto 0);

signal MSS_STAMP_3_PADDR     : std_logic_vector(31 downto 0);
signal MSS_STAMP_3_PADDR_0   : std_logic_vector(11 downto 0);
signal MSS_STAMP_3_PADDR_0_11to0: std_logic_vector(11 downto 0);

signal MSS_STAMP_4_PADDR     : std_logic_vector(31 downto 0);
signal MSS_STAMP_4_PADDR_0   : std_logic_vector(11 downto 0);
signal MSS_STAMP_4_PADDR_0_11to0: std_logic_vector(11 downto 0);

signal MSS_STAMP_5_PADDR     : std_logic_vector(31 downto 0);
signal MSS_STAMP_5_PADDR_0   : std_logic_vector(11 downto 0);
signal MSS_STAMP_5_PADDR_0_11to0: std_logic_vector(11 downto 0);


begin
----------------------------------------------------------------------
-- Constant assignments
----------------------------------------------------------------------
 GND_net                 <= '0';
 VCC_net                 <= '1';
 TM_PRDATAS6_const_net_0 <= B"00000000000000000000000000000000";
----------------------------------------------------------------------
-- Top level output port assignments
----------------------------------------------------------------------
 DAPI_TXD_net_1        <= DAPI_TXD_net_0;
 DAPI_TXD              <= DAPI_TXD_net_1;
 F_CLK_net_1           <= F_CLK_net_0;
 F_CLK                 <= F_CLK_net_1;
 F_CS1_net_1           <= F_CS1_net_0;
 F_CS1                 <= F_CS1_net_1;
 F_CS2_net_1           <= F_CS2_net_0;
 F_CS2                 <= F_CS2_net_1;
 F_MOSI_net_1          <= F_MOSI_net_0;
 F_MOSI                <= F_MOSI_net_1;
 LED_HB_MSS_net_1      <= LED_HB_MSS_net_0;
 LED_HB_MSS            <= LED_HB_MSS_net_1;
 LED_RECORDING_net_1   <= LED_RECORDING_net_0;
 LED_RECORDING         <= LED_RECORDING_net_1;
 OUT_ADC_START_net_1   <= OUT_ADC_START_net_0;
 OUT_ADC_START         <= OUT_ADC_START_net_1;
 LED_FPGA_LOADED_net_1 <= LED_FPGA_LOADED_net_0;
 LED_FPGA_LOADED       <= LED_FPGA_LOADED_net_1;
 STAMP6_CS_SGR2_net_1  <= STAMP6_CS_SGR2_net_0;
 STAMP6_CS_SGR2        <= STAMP6_CS_SGR2_net_1;
 STAMP6_CS_SGR1_net_1  <= STAMP6_CS_SGR1_net_0;
 STAMP6_CS_SGR1        <= STAMP6_CS_SGR1_net_1;
 STAMP6_MOSI_net_1     <= STAMP6_MOSI_net_0;
 STAMP6_MOSI           <= STAMP6_MOSI_net_1;
 STAMP6_SCLK_net_1     <= STAMP6_SCLK_net_0;
 STAMP6_SCLK           <= STAMP6_SCLK_net_1;
 STAMP6_CS_TEMP_net_1  <= STAMP6_CS_TEMP_net_0;
 STAMP6_CS_TEMP        <= STAMP6_CS_TEMP_net_1;
 STAMP1_CS_TEMP_net_1  <= STAMP1_CS_TEMP_net_0;
 STAMP1_CS_TEMP        <= STAMP1_CS_TEMP_net_1;
 STAMP1_MOSI_net_1     <= STAMP1_MOSI_net_0;
 STAMP1_MOSI           <= STAMP1_MOSI_net_1;
 STAMP1_SCLK_net_1     <= STAMP1_SCLK_net_0;
 STAMP1_SCLK           <= STAMP1_SCLK_net_1;
 STAMP1_CS_SGR1_net_1  <= STAMP1_CS_SGR1_net_0;
 STAMP1_CS_SGR1        <= STAMP1_CS_SGR1_net_1;
 STAMP1_CS_SGR2_net_1  <= STAMP1_CS_SGR2_net_0;
 STAMP1_CS_SGR2        <= STAMP1_CS_SGR2_net_1;
 STAMP2_CS_SGR2_net_1  <= STAMP2_CS_SGR2_net_0;
 STAMP2_CS_SGR2        <= STAMP2_CS_SGR2_net_1;
 STAMP2_SCLK_net_1     <= STAMP2_SCLK_net_0;
 STAMP2_SCLK           <= STAMP2_SCLK_net_1;
 STAMP2_CS_SGR1_net_1  <= STAMP2_CS_SGR1_net_0;
 STAMP2_CS_SGR1        <= STAMP2_CS_SGR1_net_1;
 STAMP2_MOSI_net_1     <= STAMP2_MOSI_net_0;
 STAMP2_MOSI           <= STAMP2_MOSI_net_1;
 STAMP2_CS_TEMP_net_1  <= STAMP2_CS_TEMP_net_0;
 STAMP2_CS_TEMP        <= STAMP2_CS_TEMP_net_1;
 STAMP4_CS_SGR1_net_1  <= STAMP4_CS_SGR1_net_0;
 STAMP4_CS_SGR1        <= STAMP4_CS_SGR1_net_1;
 STAMP4_CS_SGR2_net_1  <= STAMP4_CS_SGR2_net_0;
 STAMP4_CS_SGR2        <= STAMP4_CS_SGR2_net_1;
 STAMP4_MOSI_net_1     <= STAMP4_MOSI_net_0;
 STAMP4_MOSI           <= STAMP4_MOSI_net_1;
 STAMP4_SCLK_net_1     <= STAMP4_SCLK_net_0;
 STAMP4_SCLK           <= STAMP4_SCLK_net_1;
 STAMP4_CS_TEMP_net_1  <= STAMP4_CS_TEMP_net_0;
 STAMP4_CS_TEMP        <= STAMP4_CS_TEMP_net_1;
 STAMP5_MOSI_net_1     <= STAMP5_MOSI_net_0;
 STAMP5_MOSI           <= STAMP5_MOSI_net_1;
 STAMP5_CS_SGR1_net_1  <= STAMP5_CS_SGR1_net_0;
 STAMP5_CS_SGR1        <= STAMP5_CS_SGR1_net_1;
 STAMP5_CS_SGR2_net_1  <= STAMP5_CS_SGR2_net_0;
 STAMP5_CS_SGR2        <= STAMP5_CS_SGR2_net_1;
 STAMP5_SCLK_net_1     <= STAMP5_SCLK_net_0;
 STAMP5_SCLK           <= STAMP5_SCLK_net_1;
 STAMP5_CS_TEMP_net_1  <= STAMP5_CS_TEMP_net_0;
 STAMP5_CS_TEMP        <= STAMP5_CS_TEMP_net_1;
 STAMP3_CS_SGR2_net_1  <= STAMP3_CS_SGR2_net_0;
 STAMP3_CS_SGR2        <= STAMP3_CS_SGR2_net_1;
 STAMP3_SCLK_net_1     <= STAMP3_SCLK_net_0;
 STAMP3_SCLK           <= STAMP3_SCLK_net_1;
 STAMP3_CS_SGR1_net_1  <= STAMP3_CS_SGR1_net_0;
 STAMP3_CS_SGR1        <= STAMP3_CS_SGR1_net_1;
 STAMP3_MOSI_net_1     <= STAMP3_MOSI_net_0;
 STAMP3_MOSI           <= STAMP3_MOSI_net_1;
 STAMP3_CS_TEMP_net_1  <= STAMP3_CS_TEMP_net_0;
 STAMP3_CS_TEMP        <= STAMP3_CS_TEMP_net_1;
 LED_HB_MEMSYNC_net_1  <= LED_HB_MEMSYNC_net_0;
 LED_HB_MEMSYNC        <= LED_HB_MEMSYNC_net_1;
 TM_RXD_net_0          <= TM_RXD;
 TM_TXD                <= TM_RXD_net_0;
 DAPI_RTS_net_1        <= DAPI_RTS_net_0;
 DAPI_RTS              <= DAPI_RTS_net_1;
----------------------------------------------------------------------
-- Bus Interface Nets Assignments - Unequal Pin Widths
----------------------------------------------------------------------
 MSS_STAMP_PADDR_0(11 downto 0) <= ( MSS_STAMP_PADDR_0_11to0(11 downto 0) );
 MSS_STAMP_PADDR_0_11to0(11 downto 0) <= MSS_STAMP_PADDR(11 downto 0);

 MSS_STAMP_1_PADDR_0(11 downto 0) <= ( MSS_STAMP_1_PADDR_0_11to0(11 downto 0) );
 MSS_STAMP_1_PADDR_0_11to0(11 downto 0) <= MSS_STAMP_1_PADDR(11 downto 0);

 MSS_STAMP_2_PADDR_0(11 downto 0) <= ( MSS_STAMP_2_PADDR_0_11to0(11 downto 0) );
 MSS_STAMP_2_PADDR_0_11to0(11 downto 0) <= MSS_STAMP_2_PADDR(11 downto 0);

 MSS_STAMP_3_PADDR_0(11 downto 0) <= ( MSS_STAMP_3_PADDR_0_11to0(11 downto 0) );
 MSS_STAMP_3_PADDR_0_11to0(11 downto 0) <= MSS_STAMP_3_PADDR(11 downto 0);

 MSS_STAMP_4_PADDR_0(11 downto 0) <= ( MSS_STAMP_4_PADDR_0_11to0(11 downto 0) );
 MSS_STAMP_4_PADDR_0_11to0(11 downto 0) <= MSS_STAMP_4_PADDR(11 downto 0);

 MSS_STAMP_5_PADDR_0(11 downto 0) <= ( MSS_STAMP_5_PADDR_0_11to0(11 downto 0) );
 MSS_STAMP_5_PADDR_0_11to0(11 downto 0) <= MSS_STAMP_5_PADDR(11 downto 0);

----------------------------------------------------------------------
-- Component instances
----------------------------------------------------------------------
-- AND3_0
AND3_0 : AND3
    port map( 
        -- Inputs
        A => MSS_POWER_ON_RESET_N,
        B => MSS_MSS_READY,
        C => MSS_INIT_DONE,
        -- Outputs
        Y => LED_HB_MEMSYNC_net_0 
        );
-- MSS
MSS : root_sb
    port map( 
        -- Inputs
        FAB_RESET_N        => VCC_net, -- tied to '1' from definition
        STAMP_5_INTR_0_top => STAMP6_new_avail,
        STAMP_4_INTR_0_top => STAMP5_new_avail,
        STAMP_3_INTR_0_top => STAMP4_new_avail,
        STAMP_2_INTR_0_top => STAMP3_new_avail,
        STAMP_1_INTR_0_top => STAMP2_new_avail,
        STAMP_0_INTR_0_top => STAMP1_new_avail,
        TM_0_INTR_0_top    => GND_net,
        STAMP_PRDATAS0     => MSS_STAMP_PRDATA,
        STAMP_PREADYS0     => MSS_STAMP_PREADY,
        STAMP_PSLVERRS0    => MSS_STAMP_PSLVERR,
        STAMP_1_PRDATAS1   => MSS_STAMP_1_PRDATA,
        STAMP_1_PREADYS1   => MSS_STAMP_1_PREADY,
        STAMP_1_PSLVERRS1  => MSS_STAMP_1_PSLVERR,
        STAMP_2_PRDATAS2   => MSS_STAMP_2_PRDATA,
        STAMP_2_PREADYS2   => MSS_STAMP_2_PREADY,
        STAMP_2_PSLVERRS2  => MSS_STAMP_2_PSLVERR,
        STAMP_3_PRDATAS3   => MSS_STAMP_3_PRDATA,
        STAMP_3_PREADYS3   => MSS_STAMP_3_PREADY,
        STAMP_3_PSLVERRS3  => MSS_STAMP_3_PSLVERR,
        STAMP_4_PRDATAS4   => MSS_STAMP_4_PRDATA,
        STAMP_4_PREADYS4   => MSS_STAMP_4_PREADY,
        STAMP_4_PSLVERRS4  => MSS_STAMP_4_PSLVERR,
        STAMP_5_PRDATAS5   => MSS_STAMP_5_PRDATA,
        STAMP_5_PREADYS5   => MSS_STAMP_5_PREADY,
        STAMP_5_PSLVERRS5  => MSS_STAMP_5_PSLVERR,
        TM_PRDATAS6        => TM_PRDATAS6_const_net_0, -- tied to X"0" from definition
        TM_PREADYS6        => VCC_net, -- tied to '1' from definition
        TM_PSLVERRS6       => GND_net, -- tied to '0' from definition
        DEVRST_N           => DEVRST_N,
        MMUART_0_RXD_F2M   => DAPI_RXD,
        GPIO_0_F2M         => IN_RXSM_LO,
        GPIO_1_F2M         => IN_RXSM_SOE,
        GPIO_2_F2M         => IN_RXSM_SODS,
        GPIO_3_F2M         => IN_WP,
        SPI_0_DI_F2M       => F_MISO,
        SPI_0_CLK_F2M      => GND_net,
        SPI_0_SS0_F2M      => GND_net,
        -- Outputs
        POWER_ON_RESET_N   => MSS_POWER_ON_RESET_N,
        INIT_DONE          => MSS_INIT_DONE,
        STAMP_PADDRS       => MSS_STAMP_PADDR,
        STAMP_PSELS0       => MSS_STAMP_PSELx,
        STAMP_PENABLES     => MSS_STAMP_PENABLE,
        STAMP_PWRITES      => MSS_STAMP_PWRITE,
        STAMP_PWDATAS      => MSS_STAMP_PWDATA,
        STAMP_1_PADDRS     => MSS_STAMP_1_PADDR,
        STAMP_1_PSELS1     => MSS_STAMP_1_PSELx,
        STAMP_1_PENABLES   => MSS_STAMP_1_PENABLE,
        STAMP_1_PWRITES    => MSS_STAMP_1_PWRITE,
        STAMP_1_PWDATAS    => MSS_STAMP_1_PWDATA,
        STAMP_2_PADDRS     => MSS_STAMP_2_PADDR,
        STAMP_2_PSELS2     => MSS_STAMP_2_PSELx,
        STAMP_2_PENABLES   => MSS_STAMP_2_PENABLE,
        STAMP_2_PWRITES    => MSS_STAMP_2_PWRITE,
        STAMP_2_PWDATAS    => MSS_STAMP_2_PWDATA,
        STAMP_3_PADDRS     => MSS_STAMP_3_PADDR,
        STAMP_3_PSELS3     => MSS_STAMP_3_PSELx,
        STAMP_3_PENABLES   => MSS_STAMP_3_PENABLE,
        STAMP_3_PWRITES    => MSS_STAMP_3_PWRITE,
        STAMP_3_PWDATAS    => MSS_STAMP_3_PWDATA,
        STAMP_4_PADDRS     => MSS_STAMP_4_PADDR,
        STAMP_4_PSELS4     => MSS_STAMP_4_PSELx,
        STAMP_4_PENABLES   => MSS_STAMP_4_PENABLE,
        STAMP_4_PWRITES    => MSS_STAMP_4_PWRITE,
        STAMP_4_PWDATAS    => MSS_STAMP_4_PWDATA,
        STAMP_5_PADDRS     => MSS_STAMP_5_PADDR,
        STAMP_5_PSELS5     => MSS_STAMP_5_PSELx,
        STAMP_5_PENABLES   => MSS_STAMP_5_PENABLE,
        STAMP_5_PWRITES    => MSS_STAMP_5_PWRITE,
        STAMP_5_PWDATAS    => MSS_STAMP_5_PWDATA,
        TM_PADDRS          => OPEN,
        TM_PSELS6          => OPEN,
        TM_PENABLES        => OPEN,
        TM_PWRITES         => OPEN,
        TM_PWDATAS         => OPEN,
        FIC_0_CLK          => MSS_FIC_0_CLK,
        FIC_0_LOCK         => OPEN,
        MSS_READY          => MSS_MSS_READY,
        MMUART_0_TXD_M2F   => DAPI_TXD_net_0,
        GPIO_26_M2F        => DAPI_RTS_net_0,
        GPIO_28_M2F        => OUT_ADC_START_net_0,
        GPIO_29_M2F        => LED_FPGA_LOADED_net_0,
        GPIO_30_M2F        => LED_RECORDING_net_0,
        GPIO_31_M2F        => LED_HB_MSS_net_0,
        SPI_0_DO_M2F       => F_MOSI_net_0,
        SPI_0_CLK_M2F      => F_CLK_net_0,
        SPI_0_SS0_M2F      => F_CS1_net_0,
        SPI_0_SS0_M2F_OE   => OPEN,
        SPI_0_SS1_M2F      => F_CS2_net_0 
        );
-- STAMP1
STAMP1 : entity work.STAMP
    port map( 
        -- Inputs
        reset_status => GND_net,
        CLK          => MSS_FIC_0_CLK,
        RESET_N      => LED_HB_MEMSYNC_net_0,
        PSEL         => MSS_STAMP_PSELx,
        PENABLE      => MSS_STAMP_PENABLE,
        PWRITE       => MSS_STAMP_PWRITE,
        spi_miso     => STAMP1_MISO,
        ready_dms1   => STAMP1_DRDY_SGR1,
        ready_dms2   => STAMP1_DRDY_SGR2,
        ready_temp   => STAMP1_DRDY_TEMP,
        PADDR        => MSS_STAMP_PADDR_0,
        PWDATA       => MSS_STAMP_PWDATA,
        -- Outputs
        new_avail    => STAMP1_new_avail,
        PREADY       => MSS_STAMP_PREADY,
        PSLVERR      => MSS_STAMP_PSLVERR,
        spi_clock    => STAMP1_SCLK_net_0,
        spi_mosi     => STAMP1_MOSI_net_0,
        spi_dms1_cs  => STAMP1_CS_SGR1_net_0,
        spi_dms2_cs  => STAMP1_CS_SGR2_net_0,
        spi_temp_cs  => STAMP1_CS_TEMP_net_0,
        data_frame   => OPEN,
        PRDATA       => MSS_STAMP_PRDATA 
        );
-- STAMP2
STAMP2 : entity work.STAMP
    port map( 
        -- Inputs
        reset_status => GND_net,
        CLK          => MSS_FIC_0_CLK,
        RESET_N      => LED_HB_MEMSYNC_net_0,
        PSEL         => MSS_STAMP_1_PSELx,
        PENABLE      => MSS_STAMP_1_PENABLE,
        PWRITE       => MSS_STAMP_1_PWRITE,
        spi_miso     => STAMP2_MISO,
        ready_dms1   => STAMP2_DRDY_SGR1,
        ready_dms2   => STAMP2_DRDY_SGR2,
        ready_temp   => STAMP2_DRDY_TEMP,
        PADDR        => MSS_STAMP_1_PADDR_0,
        PWDATA       => MSS_STAMP_1_PWDATA,
        -- Outputs
        new_avail    => STAMP2_new_avail,
        PREADY       => MSS_STAMP_1_PREADY,
        PSLVERR      => MSS_STAMP_1_PSLVERR,
        spi_clock    => STAMP2_SCLK_net_0,
        spi_mosi     => STAMP2_MOSI_net_0,
        spi_dms1_cs  => STAMP2_CS_SGR1_net_0,
        spi_dms2_cs  => STAMP2_CS_SGR2_net_0,
        spi_temp_cs  => STAMP2_CS_TEMP_net_0,
        data_frame   => OPEN,
        PRDATA       => MSS_STAMP_1_PRDATA 
        );
-- STAMP3
STAMP3 : entity work.STAMP
    port map( 
        -- Inputs
        reset_status => GND_net,
        CLK          => MSS_FIC_0_CLK,
        RESET_N      => LED_HB_MEMSYNC_net_0,
        PSEL         => MSS_STAMP_2_PSELx,
        PENABLE      => MSS_STAMP_2_PENABLE,
        PWRITE       => MSS_STAMP_2_PWRITE,
        spi_miso     => STAMP3_MISO,
        ready_dms1   => STAMP3_DRDY_SGR1,
        ready_dms2   => STAMP3_DRDY_SGR2,
        ready_temp   => STAMP3_DRDY_TEMP,
        PADDR        => MSS_STAMP_2_PADDR_0,
        PWDATA       => MSS_STAMP_2_PWDATA,
        -- Outputs
        new_avail    => STAMP3_new_avail,
        PREADY       => MSS_STAMP_2_PREADY,
        PSLVERR      => MSS_STAMP_2_PSLVERR,
        spi_clock    => STAMP3_SCLK_net_0,
        spi_mosi     => STAMP3_MOSI_net_0,
        spi_dms1_cs  => STAMP3_CS_SGR1_net_0,
        spi_dms2_cs  => STAMP3_CS_SGR2_net_0,
        spi_temp_cs  => STAMP3_CS_TEMP_net_0,
        data_frame   => OPEN,
        PRDATA       => MSS_STAMP_2_PRDATA 
        );
-- STAMP4
STAMP4 : entity work.STAMP
    port map( 
        -- Inputs
        reset_status => GND_net,
        CLK          => MSS_FIC_0_CLK,
        RESET_N      => LED_HB_MEMSYNC_net_0,
        PSEL         => MSS_STAMP_3_PSELx,
        PENABLE      => MSS_STAMP_3_PENABLE,
        PWRITE       => MSS_STAMP_3_PWRITE,
        spi_miso     => STAMP4_MISO,
        ready_dms1   => STAMP4_DRDY_SGR1,
        ready_dms2   => STAMP4_DRDY_SGR2,
        ready_temp   => STAMP4_DRDY_TEMP,
        PADDR        => MSS_STAMP_3_PADDR_0,
        PWDATA       => MSS_STAMP_3_PWDATA,
        -- Outputs
        new_avail    => STAMP4_new_avail,
        PREADY       => MSS_STAMP_3_PREADY,
        PSLVERR      => MSS_STAMP_3_PSLVERR,
        spi_clock    => STAMP4_SCLK_net_0,
        spi_mosi     => STAMP4_MOSI_net_0,
        spi_dms1_cs  => STAMP4_CS_SGR1_net_0,
        spi_dms2_cs  => STAMP4_CS_SGR2_net_0,
        spi_temp_cs  => STAMP4_CS_TEMP_net_0,
        data_frame   => OPEN,
        PRDATA       => MSS_STAMP_3_PRDATA 
        );
-- STAMP5
STAMP5 : entity work.STAMP
    port map( 
        -- Inputs
        reset_status => GND_net,
        CLK          => MSS_FIC_0_CLK,
        RESET_N      => LED_HB_MEMSYNC_net_0,
        PSEL         => MSS_STAMP_4_PSELx,
        PENABLE      => MSS_STAMP_4_PENABLE,
        PWRITE       => MSS_STAMP_4_PWRITE,
        spi_miso     => STAMP5_MISO,
        ready_dms1   => STAMP5_DRDY_SGR1,
        ready_dms2   => STAMP5_DRDY_SGR2,
        ready_temp   => STAMP5_DRDY_TEMP,
        PADDR        => MSS_STAMP_4_PADDR_0,
        PWDATA       => MSS_STAMP_4_PWDATA,
        -- Outputs
        new_avail    => STAMP5_new_avail,
        PREADY       => MSS_STAMP_4_PREADY,
        PSLVERR      => MSS_STAMP_4_PSLVERR,
        spi_clock    => STAMP5_SCLK_net_0,
        spi_mosi     => STAMP5_MOSI_net_0,
        spi_dms1_cs  => STAMP5_CS_SGR1_net_0,
        spi_dms2_cs  => STAMP5_CS_SGR2_net_0,
        spi_temp_cs  => STAMP5_CS_TEMP_net_0,
        data_frame   => OPEN,
        PRDATA       => MSS_STAMP_4_PRDATA 
        );
-- STAMP6
STAMP6 : entity work.STAMP
    port map( 
        -- Inputs
        reset_status => GND_net,
        CLK          => MSS_FIC_0_CLK,
        RESET_N      => LED_HB_MEMSYNC_net_0,
        PSEL         => MSS_STAMP_5_PSELx,
        PENABLE      => MSS_STAMP_5_PENABLE,
        PWRITE       => MSS_STAMP_5_PWRITE,
        spi_miso     => STAMP6_MISO,
        ready_dms1   => STAMP6_DRDY_SGR1,
        ready_dms2   => STAMP6_DRDY_SGR2,
        ready_temp   => STAMP6_DRDY_TEMP,
        PADDR        => MSS_STAMP_5_PADDR_0,
        PWDATA       => MSS_STAMP_5_PWDATA,
        -- Outputs
        new_avail    => STAMP6_new_avail,
        PREADY       => MSS_STAMP_5_PREADY,
        PSLVERR      => MSS_STAMP_5_PSLVERR,
        spi_clock    => STAMP6_SCLK_net_0,
        spi_mosi     => STAMP6_MOSI_net_0,
        spi_dms1_cs  => STAMP6_CS_SGR1_net_0,
        spi_dms2_cs  => STAMP6_CS_SGR2_net_0,
        spi_temp_cs  => STAMP6_CS_TEMP_net_0,
        data_frame   => OPEN,
        PRDATA       => MSS_STAMP_5_PRDATA 
        );

end RTL;
