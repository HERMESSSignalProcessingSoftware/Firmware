set_device -family {SmartFusion2} -die {M2S010} -speed {STD}
read_vhdl -mode vhdl_2008 {B:\HERMESS_SPSoftware\Firmware\fpga\hdl\spi_master.vhd}
read_vhdl -mode vhdl_2008 {B:\HERMESS_SPSoftware\Firmware\fpga\hdl\STAMP.vhd}
read_vhdl -mode vhdl_2008 -lib COREAPB3_LIB {B:\HERMESS_SPSoftware\Firmware\fpga\component\Actel\DirectCore\CoreAPB3\4.1.100\rtl\vhdl\core\coreapb3_muxptob3.vhd}
read_vhdl -mode vhdl_2008 -lib COREAPB3_LIB {B:\HERMESS_SPSoftware\Firmware\fpga\component\Actel\DirectCore\CoreAPB3\4.1.100\rtl\vhdl\core\coreapb3_iaddr_reg.vhd}
read_vhdl -mode vhdl_2008 -lib COREAPB3_LIB {B:\HERMESS_SPSoftware\Firmware\fpga\component\Actel\DirectCore\CoreAPB3\4.1.100\rtl\vhdl\core\coreapb3.vhd}
read_vhdl -mode vhdl_2008 {B:\HERMESS_SPSoftware\Firmware\fpga\component\Actel\DirectCore\CoreResetP\7.1.100\rtl\vhdl\core\coreresetp_pcie_hotreset.vhd}
read_vhdl -mode vhdl_2008 {B:\HERMESS_SPSoftware\Firmware\fpga\component\Actel\DirectCore\CoreResetP\7.1.100\rtl\vhdl\core\coreresetp.vhd}
read_vhdl -mode vhdl_2008 {B:\HERMESS_SPSoftware\Firmware\fpga\component\work\root_sb\CCC_0\root_sb_CCC_0_FCCC.vhd}
read_vhdl -mode vhdl_2008 {B:\HERMESS_SPSoftware\Firmware\fpga\component\work\root_sb\FABOSC_0\root_sb_FABOSC_0_OSC.vhd}
read_vhdl -mode vhdl_2008 {B:\HERMESS_SPSoftware\Firmware\fpga\component\work\root_sb_MSS\root_sb_MSS.vhd}
read_vhdl -mode vhdl_2008 -lib COREAPB3_LIB {B:\HERMESS_SPSoftware\Firmware\fpga\component\Actel\DirectCore\CoreAPB3\4.1.100\rtl\vhdl\core\components.vhd}
read_vhdl -mode vhdl_2008 {B:\HERMESS_SPSoftware\Firmware\fpga\component\work\root_sb\root_sb.vhd}
read_vhdl -mode vhdl_2008 {B:\HERMESS_SPSoftware\Firmware\fpga\component\work\root\root.vhd}
set_top_level {root}
map_netlist
check_constraints {B:\HERMESS_SPSoftware\Firmware\fpga\constraint\synthesis_sdc_errors.log}
write_fdc {B:\HERMESS_SPSoftware\Firmware\fpga\designer\root\synthesis.fdc}
