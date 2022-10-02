set_component root_sb_MSS
# Microsemi Corp.
# Date: 2022-Aug-31 21:39:58
#

create_clock -period 40 [ get_pins { MSS_ADLIB_INST/CLK_CONFIG_APB } ]
set_false_path -ignore_errors -through [ get_pins { MSS_ADLIB_INST/CONFIG_PRESET_N } ]