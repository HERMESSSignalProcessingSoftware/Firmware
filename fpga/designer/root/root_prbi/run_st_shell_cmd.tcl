save
set st_prbi {C:\Users\jl\source\repos\hermess\hermess-Firmware\fpga\designer\root/root_prbi/root_st_prbi.rpt}
set fp [open $st_prbi w]
puts $fp [list_paths -from { DAPI_CTS_ibuf/U0/U_IOIN:Y } -to { Probe_Insert0 }]
puts $fp [list_paths -from { MSS/root_sb_MSS_0/MSS_ADLIB_INST/INST_MSS_010_IP:MMUART0_RTS_MGPIO17B_H2F_A } -to { Probe_Insert1 }]
close $fp
