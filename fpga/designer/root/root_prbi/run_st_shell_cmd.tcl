save
set st_prbi {C:\Users\jl\source\repos\hermess\hermess-Firmware\fpga\designer\root/root_prbi/root_st_prbi.rpt}
set fp [open $st_prbi w]
puts $fp [list_paths -from { STAMP1/spi/sclk_buffer:Q } -to { Probe_Insert1 }]
puts $fp [list_paths -from { STAMP1/spi/mosi_cl:Q } -to { Probe_Insert2 }]
close $fp
